#include "graphics.hpp"
#include "_win32/resource.h"
#include "deferred_context.hpp"
#include "resource_manager.hpp"

extern const TCHAR* g_AppWindowTitle;

using namespace boba;
using namespace bristol;


//------------------------------------------------------------------------------
Vector3 boba::ScreenToViewSpace(const Matrix& proj, u32 x, u32 y)
{
  int w, h;
  GRAPHICS.GetBackBufferSize(&w, &h);
  float xv = (2.0f * x / w - 1) / proj(0, 0);
  float yv = (-2.0f * y / h + 1) / proj(1, 1);

  return Vector3(xv, yv, 1);
}

namespace
{
  //------------------------------------------------------------------------------
  void SetClientSize(HWND hwnd, int width, int height)
  {
    RECT client_rect;
    RECT window_rect;
    GetClientRect(hwnd, &client_rect);
    GetWindowRect(hwnd, &window_rect);
    window_rect.right -= window_rect.left;
    window_rect.bottom -= window_rect.top;
    const int dx = window_rect.right - client_rect.right;
    const int dy = window_rect.bottom - client_rect.bottom;

    SetWindowPos(hwnd, NULL, -1, -1, width + dx, height + dy, SWP_NOZORDER | SWP_NOREPOSITION);
  }
}

//------------------------------------------------------------------------------
bool Graphics::SwapChain::CreateBackBuffers(u32 width, u32 height, DXGI_FORMAT format)
{
  Graphics& g = GRAPHICS;

  // Reset the buffers if the swap chain is already registered
  RenderTargetResource* rt;
  if (g._renderTargets.Get(_name, &rt))
  {
    rt->reset();
    _swapChain->ResizeBuffers(0, width, height, format, 0);
  }
  else
  {
    rt = new RenderTargetResource();
  }

  if (FAILED(_swapChain->GetBuffer(0, IID_PPV_ARGS(&rt->texture.resource))))
    return false;

  rt->texture.resource->GetDesc(&rt->texture.desc);

  // Create render target view
  D3D11_RENDER_TARGET_VIEW_DESC rtViewDesc;
  ZeroMemory(&rtViewDesc, sizeof(rtViewDesc));
  rtViewDesc.Format = rt->texture.desc.Format;
  rtViewDesc.ViewDimension = rt->texture.desc.SampleDesc.Count == 1 ? D3D11_RTV_DIMENSION_TEXTURE2D : D3D11_RTV_DIMENSION_TEXTURE2DMS;
  if (FAILED(g._device->CreateRenderTargetView(rt->texture.resource, &rtViewDesc, &rt->rtv.resource)))
    return false;

  rt->rtv.resource->GetDesc(&rt->rtv.desc);

  CD3D11_TEXTURE2D_DESC depthStencilDesc(
    DXGI_FORMAT_D24_UNORM_S8_UINT, width, height, 1, 1,
    D3D11_BIND_DEPTH_STENCIL, D3D11_USAGE_DEFAULT, 0, _desc.SampleDesc.Count);

  // Create depth stencil buffer and view
  if (FAILED(g._device->CreateTexture2D(&depthStencilDesc, NULL, &rt->depth_stencil.resource)))
    return false;

  rt->depth_stencil.resource->GetDesc(&rt->depth_stencil.desc);

  if (FAILED(g._device->CreateDepthStencilView(rt->depth_stencil.resource, NULL, &rt->dsv.resource)))
    return false;

  rt->dsv.resource->GetDesc(&rt->dsv.desc);

  // Register the buffer with GRAPHICS
  u32 idx = g._renderTargets.Insert(_name, rt);
  _renderTarget = ObjectHandle(ObjectHandle::kRenderTarget, idx);

  _viewport = CD3D11_VIEWPORT (0.0f, 0.0f, (float)width, (float)height);
  return true;
}

//------------------------------------------------------------------------------
void Graphics::SwapChain::Present()
{
  _swapChain->Present(GRAPHICS._vsync ? 1 : 0, 0);
}

//------------------------------------------------------------------------------
static ObjectHandle emptyGoh;
Graphics* Graphics::_instance;

#if WITH_DXGI_DEBUG
IDXGIDebug* Graphics::_debugInterface;
HMODULE Graphics::_debugModule;
#endif

//------------------------------------------------------------------------------
bool Graphics::EnumerateDisplayModes(HWND hWnd)
{
  vector<VideoAdapter>& videoAdapters = GRAPHICS._curSetup.videoAdapters;
  videoAdapters.clear();

  // Create DXGI factory to enumerate adapters
  if (FAILED(CreateDXGIFactory(IID_PPV_ARGS(&GRAPHICS._curSetup.dxgi_factory))))
  {
    return false;
  }

  // Enumerate the adapters
  IDXGIAdapter* adapter;
  for (int i = 0; SUCCEEDED(GRAPHICS._curSetup.dxgi_factory->EnumAdapters(i, &adapter)); ++i)
  {
    videoAdapters.push_back(VideoAdapter());
    VideoAdapter &curAdapter = videoAdapters.back();
    curAdapter.adapter = adapter;
    adapter->GetDesc(&curAdapter.desc);
    HWND hAdapter = GetDlgItem(hWnd, IDC_VIDEO_ADAPTER);
    ComboBox_AddString(hAdapter, wide_char_to_utf8(curAdapter.desc.Description).c_str());
    ComboBox_SetCurSel(hAdapter, 0);
    GRAPHICS._curSetup.selectedAdapter = 0;

    CComPtr<IDXGIOutput> output;
    vector<CComPtr<IDXGIOutput>> outputs;
    vector<DXGI_MODE_DESC> displayModes;

    // Only enumerate the first adapter
    for (int j = 0; SUCCEEDED(adapter->EnumOutputs(j, &output)); ++j)
    {
      DXGI_OUTPUT_DESC outputDesc;
      output->GetDesc(&outputDesc);
      UINT numModes;
      output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, 0, &numModes, NULL);
      size_t prevSize = displayModes.size();
      displayModes.resize(displayModes.size() + numModes);
      output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, 0, &numModes, displayModes.data() + prevSize);
      if (!GRAPHICS._displayAllModes)
        break;
    }

    // Only keep the version of each display mode with the highest refresh rate
    auto &safeRational = [](const DXGI_RATIONAL &r) { return r.Denominator == 0 ? 0 : r.Numerator / r.Denominator; };
    if (GRAPHICS.DisplayAllModes())
    {
      curAdapter.displayModes = displayModes;
    }
    else
    {
      map<pair<int, int>, DXGI_RATIONAL> highestRate;
      for (size_t i = 0; i < displayModes.size(); ++i)
      {
        auto &cur = displayModes[i];
        auto key = make_pair(cur.Width, cur.Height);
        if (safeRational(cur.RefreshRate) > safeRational(highestRate[key]))
        {
          highestRate[key] = cur.RefreshRate;
        }
      }

      for (auto it = begin(highestRate); it != end(highestRate); ++it)
      {
        DXGI_MODE_DESC desc;
        desc.Width = it->first.first;
        desc.Height = it->first.second;
        desc.RefreshRate = it->second;
        curAdapter.displayModes.push_back(desc);
      }

      auto &resSorter = [&](const DXGI_MODE_DESC &a, const DXGI_MODE_DESC &b)
      { 
        return a.Width < b.Width;
      };

      sort(begin(curAdapter.displayModes), end(curAdapter.displayModes), resSorter);
    }

    HWND hDisplayMode = GetDlgItem(hWnd, IDC_DISPLAY_MODES);
    for (size_t k = 0; k < curAdapter.displayModes.size(); ++k)
    {
      auto &cur = curAdapter.displayModes[k];
      char buf[256];
      sprintf(buf, "%dx%d (%dHz)", cur.Width, cur.Height, safeRational(cur.RefreshRate));
      ComboBox_InsertString(hDisplayMode, k, buf);
      // encode width << 16 | height in the item data
      ComboBox_SetItemData(hDisplayMode, k, (cur.Width << 16) | cur.Height);
    }

    int cnt = ComboBox_GetCount(hDisplayMode);
    ComboBox_SetCurSel(hDisplayMode, cnt-1);

    adapter->Release();
  }

  HWND hMultisample = GetDlgItem(hWnd, IDC_MULTISAMPLE);
  ComboBox_InsertString(hMultisample, -1, "No multisample");
  ComboBox_InsertString(hMultisample, -1, "2x multisample");
  ComboBox_InsertString(hMultisample, -1, "4x multisample");
  ComboBox_InsertString(hMultisample, -1, "8x multisample");

  ComboBox_SetItemData(hMultisample, 0, 1);
  ComboBox_SetItemData(hMultisample, 1, 2);
  ComboBox_SetItemData(hMultisample, 2, 4);
  ComboBox_SetItemData(hMultisample, 3, 8);

  ComboBox_SetCurSel(hMultisample, 0);
  GRAPHICS._curSetup.multisampleCount = 1;

#if DISTRIBUTION
  GRAPHICS._curSetup.windowed = false;
#else
  GRAPHICS._curSetup.windowed = true;
  Button_SetCheck(GetDlgItem(hWnd, IDC_WINDOWED), TRUE);
#endif

  return true;
}

//------------------------------------------------------------------------------
INT_PTR CALLBACK Graphics::dialogWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message)
  {
    case WM_INITDIALOG:
    {
      if (!EnumerateDisplayModes(hWnd))
      {
        EndDialog(hWnd, -1);
      }
#if !WITH_CONFIG_DLG
      if (!GRAPHICS._curSetup.videoAdapters.empty())
      {
        ComboBox_SetCurSel(GetDlgItem(hWnd, IDC_DISPLAY_MODES), 3 * GRAPHICS._curSetup.videoAdapters[0].displayModes.size() / 4);
      }
      EndDialog(hWnd, 1);
#endif
      RECT rect;
      GetClientRect(hWnd, &rect);
      int width = GetSystemMetrics(SM_CXSCREEN);
      int height = GetSystemMetrics(SM_CYSCREEN);
      SetWindowPos(hWnd, NULL, width/2 - (rect.right - rect.left) / 2, height/2 - (rect.bottom - rect.top)/2, -1, -1, 
        SWP_NOZORDER | SWP_NOSIZE);
      break;
    }

    case WM_COMMAND:
      switch (LOWORD(wParam))
      {
        case IDCANCEL:
          EndDialog(hWnd, 0);
          return TRUE;

        case IDOK:
          EndDialog(hWnd, 1);
          return TRUE;
      }
      break; // end WM_COMMAND

    case WM_DESTROY:
    {
      // read the settings
      Graphics::Setup& cur = GRAPHICS._curSetup;
      cur.windowed = !!Button_GetCheck(GetDlgItem(hWnd, IDC_WINDOWED));
      cur.selectedDisplayMode = ComboBox_GetCurSel(GetDlgItem(hWnd, IDC_DISPLAY_MODES));

      HWND hMultisample = GetDlgItem(hWnd, IDC_MULTISAMPLE);
      cur.multisampleCount = (int)ComboBox_GetItemData(hMultisample, ComboBox_GetCurSel(hMultisample));

      HWND hDisplayModes = GetDlgItem(hWnd, IDC_DISPLAY_MODES);
      int sel = ComboBox_GetCurSel(hDisplayModes);
      int data = (int)ComboBox_GetItemData(hDisplayModes, sel);
      cur.width = HIWORD(data);
      cur.height = LOWORD(data);
      break;
    }
  }
  return FALSE;
}


//------------------------------------------------------------------------------
Graphics::Graphics()
  : _vsProfile("vs_4_0")
  , _psProfile("ps_4_0")
  , _csProfile("cs_4_0")
  , _gsProfile("gs_4_0")
  , _vertexShaders(ReleaseObj<ID3D11VertexShader *>)
  , _pixelShaders(ReleaseObj<ID3D11PixelShader *>)
  , _computeShaders(ReleaseObj<ID3D11ComputeShader *>)
  , _geometryShaders(ReleaseObj<ID3D11GeometryShader *>)
  , _vertexBuffers(ReleaseObj<ID3D11Buffer *>)
  , _indexBuffers(ReleaseObj<ID3D11Buffer *>)
  , _constantBuffers(ReleaseObj<ID3D11Buffer *>)
  , _inputLayouts(ReleaseObj<ID3D11InputLayout *>)
  , _blendStates(ReleaseObj<ID3D11BlendState *>)
  , _depthStencilStates(ReleaseObj<ID3D11DepthStencilState *>)
  , _rasterizerStates(ReleaseObj<ID3D11RasterizerState *>)
  , _sampler_states(ReleaseObj<ID3D11SamplerState *>)
  , _shaderResourceViews(ReleaseObj<ID3D11ShaderResourceView *>)
  , _textures(DeleteObj<TextureResource *>)
  , _renderTargets(DeleteObj<RenderTargetResource *>)
  , _resources(DeleteObj<SimpleResource *>)
  , _structuredBuffers(DeleteObj<StructuredBuffer *>)
  , _swapChains(DeleteObj<SwapChain*>)
  , _vsync(true)
  , _totalBytesAllocated(0)
  , _displayAllModes(false)
{
}

//------------------------------------------------------------------------------
Graphics::~Graphics()
{
}

//------------------------------------------------------------------------------
bool Graphics::Create(HINSTANCE hInstance)
{
  assert(!_instance);
  _instance = new Graphics();

#if WITH_DXGI_DEBUG
  // Get the DXGIGetDebugInterface
  if (_debugModule = LoadLibraryA("Dxgidebug.dll"))
  {
    typedef HRESULT (WINAPI *DXGIGetDebugInterfaceFunc)(REFIID, void**);
    auto fn = (DXGIGetDebugInterfaceFunc)GetProcAddress(_debugModule, "DXGIGetDebugInterface");
    fn(__uuidof(IDXGIDebug), (void**)&_debugInterface);
  }
#endif

  return _instance->Init(hInstance);
}

//------------------------------------------------------------------------------
bool Graphics::Init(HINSTANCE hInstance)
{
  _hInstance = hInstance;
  int res = (int)DialogBox(hInstance, MAKEINTRESOURCE(IDD_SETUP_DLG), NULL, dialogWndProc);
  if (res != IDOK)
  {
    return false;
  }

  if (!CreateDevice())
    return false;

  _defaultDepthStencilState = CreateDepthStencilState(CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT()));
  _defaultRasterizerState = CreateRasterizerState(CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT()));
  _defaultBlendState = CreateBlendState(CD3D11_BLEND_DESC(CD3D11_DEFAULT()));

  CreateDefaultGeometry();

  // Create a dummy texture
  DWORD black = 0;
  _dummyTexture = CreateTexture(1, 1, DXGI_FORMAT_R8G8B8A8_UNORM, &black, 1, 1, 1, "dummy_texture");
  _immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  for (int i = 0; i < 4; ++i)
    _defaultBlendFactors[i] = 1.0f;

#if WITH_FONT_RENDERING
  HRESULT hResult = FW1CreateFactory(FW1_VERSION, &_fw1Factory);
  hResult = _fw1Factory->CreateFontWrapper(_device, L"Arial", &_fw1FontWrapper);
#endif

   return true;
}

//------------------------------------------------------------------------------
bool Graphics::Destroy()
{
  delete exch_null(_instance);
#if WITH_DXGI_DEBUG
  if (_debugModule)
  {
    OutputDebugStringA("** Dumping live objects\n");
    // todo: figure this out
    //_debugInterface->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
    FreeLibrary(_debugModule);
  }
#endif

  return true;
}

//------------------------------------------------------------------------------
const DXGI_MODE_DESC &Graphics::selectedDisplayMode() const
{
  return _curSetup.videoAdapters[_curSetup.selectedAdapter].displayModes[_curSetup.selectedDisplayMode];
}

//------------------------------------------------------------------------------
bool Graphics::CreateDevice()
{
  int flags = 0;
#if WITH_DXGI_DEBUG
  flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

  flags |= D3D11_CREATE_DEVICE_BGRA_SUPPORT;

  // Create the DX11 device
  CComPtr<IDXGIAdapter> adapter = _curSetup.videoAdapters[_curSetup.selectedAdapter].adapter;
  if (FAILED(D3D11CreateDevice(
      adapter,D3D_DRIVER_TYPE_UNKNOWN, NULL, flags, NULL, 0, D3D11_SDK_VERSION,
      &_device, &_featureLevel, &_immediateContext)))
    return false;

  if (_featureLevel < D3D_FEATURE_LEVEL_9_3)
    return false;

#if WITH_DXGI_DEBUG
  if (FAILED(_device->QueryInterface(IID_ID3D11Debug, (void **)&(_d3dDebug.p))))
    return false;
#endif

  return true;
}

//------------------------------------------------------------------------------
ObjectHandle Graphics::CreateBuffer(
    D3D11_BIND_FLAG bind,
    int size,
    bool dynamic,
    const void* buf,
    int userData)
{
  ID3D11Buffer* buffer = 0;
  if (CreateBufferInner(bind, size, dynamic, buf, &buffer))
  {
    if (bind == D3D11_BIND_INDEX_BUFFER)
    {
      const int idx = _indexBuffers.Insert(buffer);
      assert(userData == DXGI_FORMAT_R16_UINT || userData == DXGI_FORMAT_R32_UINT);
      return MakeObjectHandle(ObjectHandle::kIndexBuffer, idx, userData);
    } 
    else if (bind == D3D11_BIND_VERTEX_BUFFER)
    {
      const int idx = _vertexBuffers.Insert(buffer);
      // userdata is vertex size
      assert(userData > 0);
      return MakeObjectHandle(ObjectHandle::kVertexBuffer, idx, userData);
    } 
    else if (bind == D3D11_BIND_CONSTANT_BUFFER)
    {
      const int idx = _constantBuffers.Insert(buffer);
      return MakeObjectHandle(ObjectHandle::kConstantBuffer, idx, size);
    }
    else
    {
      //LOG_ERROR_LN("Implement me!");
    }
  }
  return emptyGoh;
}

//------------------------------------------------------------------------------
bool Graphics::CreateBufferInner(
    D3D11_BIND_FLAG bind,
    int size,
    bool dynamic,
    const void* data,
    ID3D11Buffer** buffer)
{
  CD3D11_BUFFER_DESC desc(((size + 15) & ~0xf), bind, 
    dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT, 
    dynamic ? D3D11_CPU_ACCESS_WRITE : 0);

  D3D11_SUBRESOURCE_DATA init_data;
  ZeroMemory(&init_data, sizeof(init_data));
  init_data.pSysMem = data;
  HRESULT hr = _device->CreateBuffer(&desc, data ? &init_data : NULL, buffer);
  if (SUCCEEDED(hr))
  {
    _totalBytesAllocated += size;
  }
  return SUCCEEDED(hr);
}

//------------------------------------------------------------------------------
ID3D11ShaderResourceView* Graphics::GetShaderResourceView(ObjectHandle h)
{
  ObjectHandle::Type type = h.type();

  if (type == ObjectHandle::kTexture)
  {
    return _textures.Get(h)->view.resource;
  }
  else if (type == ObjectHandle::kResource)
  {
    return _resources.Get(h)->view.resource;
  }
  else if (type == ObjectHandle::kRenderTarget)
  {
    return _renderTargets.Get(h)->srv.resource;
  }
  else if (type == ObjectHandle::kStructuredBuffer)
  {
    return _structuredBuffers.Get(h)->srv.resource;
  }
  return nullptr;
}

//------------------------------------------------------------------------------
bool Graphics::GetTextureSize(ObjectHandle h, u32* x, u32* y)
{
  ObjectHandle::Type type = h.type();

  if (type == ObjectHandle::kTexture)
  {
    *x = _textures.Get(h)->texture.desc.Width;
    *y = _textures.Get(h)->texture.desc.Height;
    return true;
  }
  else if (type == ObjectHandle::kResource)
  {
    u32 mipLevel = _resources.Get(h)->view.desc.Texture2D.MostDetailedMip;

    CComPtr<ID3D11Texture2D> texture;
    D3D11_TEXTURE2D_DESC desc;
    texture.Attach((ID3D11Texture2D*)_resources.Get(h)->view.resource.p);
    texture->GetDesc(&desc);

    *x = max(desc.Width / (1 << mipLevel), 1u);
    *y = max(desc.Height / (1 << mipLevel), 1u);
    return true;
  }
  else if (type == ObjectHandle::kRenderTarget)
  {
    const D3D11_RENDER_TARGET_VIEW_DESC& rtDesc = _renderTargets.Get(h)->rtv.desc;
    const D3D11_TEXTURE2D_DESC& desc = _renderTargets.Get(h)->texture.desc;
    u32 mipLevel = rtDesc.Texture2D.MipSlice;
    *x = max(desc.Width / (1 << mipLevel), 1u);
    *y = max(desc.Height / (1 << mipLevel), 1u);
    return true;
  }
  else if (type == ObjectHandle::kStructuredBuffer)
  {
    assert(false);
    return true;
//    return _structuredBuffers.Get(h)->srv.resource;
  }

  return false;
}

//------------------------------------------------------------------------------
ObjectHandle Graphics::GetTempRenderTarget(
  int width,
  int height,
  DXGI_FORMAT format,
  const BufferFlags& bufferFlags)
{
  // look for a free render target with the wanted properties
  UINT flags = bufferFlags.IsSet(BufferFlag::CreateMipMaps) ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;

  auto rtComp = [&](const RenderTargetResource* res)
  {
    auto &desc = res->texture.desc;
    return 
      !res->in_use
      && desc.Width == width
      && desc.Height == height
      && desc.Format == format
      && desc.MiscFlags == flags
      && bufferFlags.IsSet(BufferFlag::CreateDepthBuffer) == (res->depth_stencil.resource.p != nullptr);
  };

  int idx = _renderTargets.Find(rtComp);
  if (idx != ~0)
  {
    RenderTargetResource* rt = _renderTargets.Get(idx);
    rt->in_use = true;
    return MakeObjectHandle(ObjectHandle::kRenderTarget, idx);
  }

  // nothing suitable found, so we create a render target
  return CreateRenderTarget(width, height, format, bufferFlags);
}

//------------------------------------------------------------------------------
void Graphics::ReleaseTempRenderTarget(ObjectHandle h)
{
  RenderTargetResource* rt = _renderTargets.Get(h);
  assert(rt->in_use);
  rt->in_use = false;
}

//------------------------------------------------------------------------------
ObjectHandle Graphics::CreateStructuredBuffer(
    int elemSize,
    int numElems,
    bool createSrv)
{
  unique_ptr<StructuredBuffer> sb(new StructuredBuffer);

  // Create Structured Buffer
  D3D11_BUFFER_DESC sbDesc;
  sbDesc.BindFlags            = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
  sbDesc.CPUAccessFlags       = 0;
  sbDesc.MiscFlags            = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
  sbDesc.StructureByteStride  = elemSize;
  sbDesc.ByteWidth            = elemSize * numElems;
  sbDesc.Usage                = D3D11_USAGE_DEFAULT;
  if (FAILED(_device->CreateBuffer(&sbDesc, NULL, &sb->buffer.resource)))
    return emptyGoh;

  auto buf = sb->buffer.resource.p;

  // create the UAV for the structured buffer
  D3D11_UNORDERED_ACCESS_VIEW_DESC sbUAVDesc;
  sbUAVDesc.Buffer.FirstElement       = 0;
  sbUAVDesc.Buffer.Flags              = 0;
  sbUAVDesc.Buffer.NumElements        = numElems;
  sbUAVDesc.Format                    = DXGI_FORMAT_UNKNOWN;
  sbUAVDesc.ViewDimension             = D3D11_UAV_DIMENSION_BUFFER;
  if (FAILED(_device->CreateUnorderedAccessView(buf, &sbUAVDesc, &sb->uav.resource)))
    return emptyGoh;

  if (createSrv)
  {
    // create the Shader Resource View (SRV) for the structured buffer
    D3D11_SHADER_RESOURCE_VIEW_DESC sbSRVDesc;
    sbSRVDesc.Buffer.ElementOffset          = 0;
    sbSRVDesc.Buffer.ElementWidth           = elemSize;
    sbSRVDesc.Buffer.FirstElement           = 0;
    sbSRVDesc.Buffer.NumElements            = numElems;
    sbSRVDesc.Format                        = DXGI_FORMAT_UNKNOWN;
    sbSRVDesc.ViewDimension                 = D3D11_SRV_DIMENSION_BUFFER;
    if (FAILED(_device->CreateShaderResourceView(buf, &sbSRVDesc, &sb->srv.resource)))
      return emptyGoh;
  }

  return MakeObjectHandle(
      ObjectHandle::kStructuredBuffer, _structuredBuffers.Insert(sb.release()));
}

//------------------------------------------------------------------------------
ObjectHandle Graphics::CreateRenderTarget(
    int width,
    int height,
    DXGI_FORMAT format,
    const BufferFlags& bufferFlags,
    const string& name)
{
  ObjectHandle goh;
  unique_ptr<RenderTargetResource> data(new RenderTargetResource);
  if (CreateRenderTarget(width, height, format, bufferFlags, data.get()))
  {
    int idx = name.empty()
      ? _renderTargets.Insert(data.release())
      : _renderTargets.Insert(name, data.release());
    return MakeObjectHandle(ObjectHandle::kRenderTarget, idx);
  }
  return emptyGoh;
}

//------------------------------------------------------------------------------
bool Graphics::CreateRenderTarget(
    int width,
    int height,
    DXGI_FORMAT format,
    const BufferFlags& bufferFlags,
    RenderTargetResource *out)
{
  out->reset();
  out->in_use = true;

  // create the render target
  int mip_levels = bufferFlags.IsSet(BufferFlag::CreateMipMaps) ? 0 : 1;
  u32 flags = D3D11_BIND_RENDER_TARGET 
    | (bufferFlags.IsSet(BufferFlag::CreateSrv) ? D3D11_BIND_SHADER_RESOURCE : 0)
    | (bufferFlags.IsSet(BufferFlag::CreateUav) ? D3D11_BIND_UNORDERED_ACCESS : 0);

  out->texture.desc = CD3D11_TEXTURE2D_DESC(format, width, height, 1, mip_levels, flags);
  out->texture.desc.MiscFlags = bufferFlags.IsSet(BufferFlag::CreateMipMaps) ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;
  if (FAILED(_device->CreateTexture2D(&out->texture.desc, NULL, &out->texture.resource.p)))
    return false;

  // create the render target view
  out->rtv.desc = CD3D11_RENDER_TARGET_VIEW_DESC(D3D11_RTV_DIMENSION_TEXTURE2D, out->texture.desc.Format);
  if (FAILED(_device->CreateRenderTargetView(out->texture.resource, &out->rtv.desc, &out->rtv.resource.p)))
    return false;

  if (bufferFlags.IsSet(BufferFlag::CreateDepthBuffer))
  {
    // create the depth stencil texture
    out->depth_stencil.desc = CD3D11_TEXTURE2D_DESC(DXGI_FORMAT_D24_UNORM_S8_UINT, width, height, 1, 1, D3D11_BIND_DEPTH_STENCIL);
    if (FAILED(_device->CreateTexture2D(&out->depth_stencil.desc, NULL, &out->depth_stencil.resource.p)))
      return false;

    // create depth stencil view
    out->dsv.desc = CD3D11_DEPTH_STENCIL_VIEW_DESC(D3D11_DSV_DIMENSION_TEXTURE2D, DXGI_FORMAT_D24_UNORM_S8_UINT);
    if (FAILED(_device->CreateDepthStencilView(out->depth_stencil.resource, &out->dsv.desc, &out->dsv.resource.p)))
      return false;
  }

  if (bufferFlags.IsSet(BufferFlag::CreateSrv))
  {
    // create the shader resource view
    out->srv.desc = CD3D11_SHADER_RESOURCE_VIEW_DESC(D3D11_SRV_DIMENSION_TEXTURE2D, out->texture.desc.Format);
    if (FAILED(_device->CreateShaderResourceView(out->texture.resource, &out->srv.desc, &out->srv.resource.p)))
      return false;
  }

  if (bufferFlags.IsSet(BufferFlag::CreateUav))
  {
    out->uav.desc = CD3D11_UNORDERED_ACCESS_VIEW_DESC(D3D11_UAV_DIMENSION_TEXTURE2D, format, 0, 0, width*height);
    if (FAILED(_device->CreateUnorderedAccessView(out->texture.resource, &out->uav.desc, &out->uav.resource)))
      return false;
  }

  return true;
}

//------------------------------------------------------------------------------
bool Graphics::ReadTexture(
    const char *filename,
    D3DX11_IMAGE_INFO *info,
    u32 *pitch,
    vector<u8> *bits)
{
  HRESULT hr;
  D3DX11GetImageInfoFromFileA(filename, NULL, info, &hr);
  if (FAILED(hr))
    return false;

  D3DX11_IMAGE_LOAD_INFO loadinfo;
  ZeroMemory(&loadinfo, sizeof(D3DX11_IMAGE_LOAD_INFO));
  loadinfo.CpuAccessFlags = D3D11_CPU_ACCESS_READ;
  loadinfo.Usage = D3D11_USAGE_STAGING;
  loadinfo.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  CComPtr<ID3D11Resource> resource;
  D3DX11CreateTextureFromFileA(_device, filename, &loadinfo, NULL, &resource.p, &hr);
  if (FAILED(hr))
    return false;

  D3D11_MAPPED_SUBRESOURCE sub;
  if (FAILED(_immediateContext->Map(resource, 0, D3D11_MAP_READ, 0, &sub)))
    return false;

  u8 *src = (u8 *)sub.pData;
  bits->resize(sub.RowPitch * info->Height);
  u8 *dst = &(*bits)[0];

  int row_size = 4 * info->Width;
  for (u32 i = 0; i < info->Height; ++i)
  {
    memcpy(&dst[i*sub.RowPitch], &src[i*sub.RowPitch], row_size);
  }

  _immediateContext->Unmap(resource, 0);
  *pitch = sub.RowPitch;
  return true;
}

//------------------------------------------------------------------------------
ObjectHandle Graphics::GetTexture(const char *filename)
{
  return MakeObjectHandle(ObjectHandle::kResource, _resources.IndexFromKey(filename));
}

//------------------------------------------------------------------------------
ObjectHandle Graphics::LoadTexture(
    const char* filename,
    const char* friendlyName,
    bool srgb,
    D3DX11_IMAGE_INFO *info)
{
  D3DX11_IMAGE_INFO imageInfo;
  if (FAILED(D3DX11GetImageInfoFromFileA(filename, NULL, &imageInfo, NULL)))
    return emptyGoh;

  if (info)
    *info = imageInfo;

  auto data = unique_ptr<SimpleResource>(new SimpleResource());
  if (FAILED(D3DX11CreateTextureFromFileA(_device, filename, NULL, NULL, &data->resource, NULL)))
    return emptyGoh;

  // TODO: allow for srgb loading
  auto fmt = DXGI_FORMAT_R8G8B8A8_UNORM;
  // check if this is a cube map
  auto dim = imageInfo.MiscFlags & D3D11_RESOURCE_MISC_TEXTURECUBE ? D3D11_SRV_DIMENSION_TEXTURECUBE : D3D11_SRV_DIMENSION_TEXTURE2D;
  auto desc = CD3D11_SHADER_RESOURCE_VIEW_DESC(dim, fmt);
  if (FAILED(_device->CreateShaderResourceView(data->resource, &desc, &data->view.resource)))
    return emptyGoh;

  return MakeObjectHandle(ObjectHandle::kResource, 
    _resources.Insert(friendlyName ? friendlyName : filename, data.release()));
}

//------------------------------------------------------------------------------
ObjectHandle Graphics::LoadTextureFromMemory(
    const void *buf,
    u32 len,
    const char *friendlyName,
    bool srgb,
    D3DX11_IMAGE_INFO *info)
{
  HRESULT hr;
  if (info && FAILED(D3DX11GetImageInfoFromMemory(buf, len, NULL, info, &hr)))
    return emptyGoh;

  auto data = unique_ptr<SimpleResource>(new SimpleResource());
  if (FAILED(D3DX11CreateTextureFromMemory(_device, buf, len, NULL, NULL, &data->resource, &hr)))
  {
    return emptyGoh;
  }

  // TODO: allow for srgb loading
  auto desc = CD3D11_SHADER_RESOURCE_VIEW_DESC(D3D11_SRV_DIMENSION_TEXTURE2D, DXGI_FORMAT_R8G8B8A8_UNORM);
  if (FAILED(_device->CreateShaderResourceView(data->resource, &desc, &data->view.resource)))
    return emptyGoh;

  return MakeObjectHandle(
      ObjectHandle::kResource, _resources.Insert(friendlyName, data.release()));
}

//------------------------------------------------------------------------------
ObjectHandle Graphics::InsertTexture(
    TextureResource *data,
    const char *friendlyName)
{
  return MakeObjectHandle(
      ObjectHandle::kTexture, _textures.Insert(friendlyName, data));
}

//------------------------------------------------------------------------------
ObjectHandle Graphics::CreateTexture(
    const D3D11_TEXTURE2D_DESC &desc,
    const char *name)
{
  TextureResource *data = new TextureResource;
  if (!CreateTexture(desc, data))
  {
    delete exch_null(data);
    return emptyGoh;
  }
  return InsertTexture(data, name);
}

//------------------------------------------------------------------------------
bool Graphics::CreateTexture(
    const D3D11_TEXTURE2D_DESC &desc,
    TextureResource *out)
{
  out->reset();

  // create the texture
  out->texture.desc = desc;
  if (FAILED(_device->CreateTexture2D(&out->texture.desc, NULL, &out->texture.resource.p)))
    return false;

  // create the shader resource view if the texture has a shader resource bind flag
  if (desc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
  {
    out->view.desc = CD3D11_SHADER_RESOURCE_VIEW_DESC(D3D11_SRV_DIMENSION_TEXTURE2D, out->texture.desc.Format);
    if (FAILED(_device->CreateShaderResourceView(out->texture.resource, &out->view.desc, &out->view.resource.p)))
      return false;
  }

  return true;
}

//------------------------------------------------------------------------------
ObjectHandle Graphics::CreateTexture(
    int width,
    int height,
    DXGI_FORMAT fmt, 
    void *data_bits,
    int data_width,
    int data_height,
    int data_pitch,
    const char *friendlyName)
{
  TextureResource *data = new TextureResource;
  if (!CreateTexture(width, height, fmt, data_bits, data_width, data_height, data_pitch, data))
  {
    delete exch_null(data);
    return emptyGoh;
  }
  return InsertTexture(data, friendlyName);
}

//------------------------------------------------------------------------------
bool Graphics::CreateTexture(
    int width,
    int height,
    DXGI_FORMAT fmt,
    void *data,
    int data_width,
    int data_height,
    int data_pitch,
    TextureResource *out)
{
  if (!CreateTexture(CD3D11_TEXTURE2D_DESC(fmt, width, height, 1, 1, D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE), out))
    return false;

  D3D11_MAPPED_SUBRESOURCE resource;
  if (FAILED(_immediateContext->Map(out->texture.resource, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource)))
    return false;

  uint8_t *src = (uint8_t *)data;
  uint8_t *dst = (uint8_t *)resource.pData;
  const int w = std::min<int>(width, data_width);
  const int h = std::min<int>(height, data_height);
  for (int i = 0; i < h; ++i)
  {
    memcpy(dst, src, w);
    src += data_pitch;
    dst += resource.RowPitch;
  }
  _immediateContext->Unmap(out->texture.resource, 0);
  return true;
}

//------------------------------------------------------------------------------
bool Graphics::CreateDefaultGeometry()
{
  static const u16 quadIndices[] =
  {
    0, 1, 2,
    2, 1, 3
  };

  {
    // fullscreen pos/tex quad
    static const float verts[] =
    {
      -1, +1, +1, +1, +0, +0,
      +1, +1, +1, +1, +1, +0,
      -1, -1, +1, +1, +0, +1,
      +1, -1, +1, +1, +1, +1
    };

    auto vb = CreateBuffer(D3D11_BIND_VERTEX_BUFFER, sizeof(verts), false, verts, sizeof(Pos4Tex));
    auto ib = CreateBuffer(D3D11_BIND_INDEX_BUFFER, sizeof(quadIndices), false, quadIndices, DXGI_FORMAT_R16_UINT);
    _predefinedGeometry.insert(make_pair(kGeomFsQuadPosTex, make_pair(vb, ib)));
  }

  // fullscreen pos quad
  {
    static const float verts[] =
    {
      -1, +1, +1, 1,
      +1, +1, +1, 1,
      -1, -1, +1, 1,
      +1, -1, +1, 1,
    };

    auto vb = CreateBuffer(D3D11_BIND_VERTEX_BUFFER, sizeof(verts), false, verts, sizeof(Vector4));
    auto ib = CreateBuffer(D3D11_BIND_INDEX_BUFFER, sizeof(quadIndices), false, quadIndices, DXGI_FORMAT_R16_UINT);
    _predefinedGeometry.insert(make_pair(kGeomFsQuadPos, make_pair(vb, ib)));
  }

  return true;
}

//------------------------------------------------------------------------------
ObjectHandle Graphics::CreateInputLayout(
    const vector<D3D11_INPUT_ELEMENT_DESC> &desc,
    const vector<char> &shader_bytecode)
{
  ID3D11InputLayout* layout = nullptr;
  if (FAILED(_device->CreateInputLayout(&desc[0], (u32)desc.size(), &shader_bytecode[0], shader_bytecode.size(), &layout)))
  {
    LOG_WARN("Error creating input layout");
    return emptyGoh;
  }

  return ObjectHandle(ObjectHandle::kInputLayout, _inputLayouts.Insert(layout));
}

//------------------------------------------------------------------------------
template<typename T, class Cont>
ObjectHandle AddShader(
    Cont &cont,
    T *shader,
    const string &id,
    ObjectHandle::Type type)
{
  return Graphics::MakeObjectHandle(type, cont.Insert(id, shader));
}

//------------------------------------------------------------------------------
ObjectHandle Graphics::CreateVertexShader(
    const vector<char> &shader_bytecode,
    const string &id)
{
  ID3D11VertexShader *vs = nullptr;
  if (SUCCEEDED(_device->CreateVertexShader(&shader_bytecode[0], shader_bytecode.size(), NULL, &vs)))
  {
    return AddShader(_vertexShaders, vs, id, ObjectHandle::kVertexShader);
  }
  return emptyGoh;
}

//------------------------------------------------------------------------------
ObjectHandle Graphics::CreatePixelShader(
    const vector<char> &shader_bytecode,
    const string &id)
{
  ID3D11PixelShader *ps = nullptr;
  if (SUCCEEDED(_device->CreatePixelShader(&shader_bytecode[0], shader_bytecode.size(), NULL, &ps)))
  {
    return AddShader(_pixelShaders, ps, id, ObjectHandle::kPixelShader);
  }
  return emptyGoh;
}

//------------------------------------------------------------------------------
ObjectHandle Graphics::CreateComputeShader(
    const vector<char> &shader_bytecode,
    const string &id)
{
  ID3D11ComputeShader *cs = nullptr;
  if (SUCCEEDED(_device->CreateComputeShader(&shader_bytecode[0], shader_bytecode.size(), NULL, &cs)))
  {
    return AddShader(_computeShaders, cs, id, ObjectHandle::kComputeShader);
  }
  return emptyGoh;
}

//------------------------------------------------------------------------------
ObjectHandle Graphics::CreateGeometryShader(
    const vector<char> &shader_bytecode,
    const string &id)
{
  ID3D11GeometryShader *cs = nullptr;
  if (SUCCEEDED(_device->CreateGeometryShader(&shader_bytecode[0], shader_bytecode.size(), NULL, &cs)))
  {
    return AddShader(_geometryShaders, cs, id, ObjectHandle::kGeometryShader);
  }
  return emptyGoh;
}

//------------------------------------------------------------------------------
ObjectHandle Graphics::CreateRasterizerState(
    const D3D11_RASTERIZER_DESC &desc,
    const char *name)
{
  ID3D11RasterizerState *rs;
  if (SUCCEEDED(_device->CreateRasterizerState(&desc, &rs)))
  {
    return MakeObjectHandle(ObjectHandle::kRasterizerState,
        name 
          ? _rasterizerStates.Insert(name, rs)
          : _rasterizerStates.Insert(rs));
  }
  return emptyGoh;
}

//------------------------------------------------------------------------------
ObjectHandle Graphics::CreateBlendState(
    const D3D11_BLEND_DESC &desc,
    const char *name)
{
  ID3D11BlendState *bs;
  if (SUCCEEDED(_device->CreateBlendState(&desc, &bs)))
  {
    return MakeObjectHandle(ObjectHandle::kBlendState,
        name 
          ? _blendStates.Insert(name, bs)
          : _blendStates.Insert(bs));
  }
  return emptyGoh;
}

//------------------------------------------------------------------------------
ObjectHandle Graphics::CreateDepthStencilState(
    const D3D11_DEPTH_STENCIL_DESC &desc,
    const char *name)
{
  ID3D11DepthStencilState *dss;
  if (SUCCEEDED(_device->CreateDepthStencilState(&desc, &dss)))
  {
    return MakeObjectHandle(ObjectHandle::kDepthStencilState,
        name 
          ? _depthStencilStates.Insert(name, dss)
          : _depthStencilStates.Insert(dss));
  }
  return emptyGoh;
}

//------------------------------------------------------------------------------
ObjectHandle Graphics::CreateSamplerState(
    const D3D11_SAMPLER_DESC &desc,
    const char *name)
{
  ID3D11SamplerState *ss;
  if (SUCCEEDED(_device->CreateSamplerState(&desc, &ss)))
  {
    return MakeObjectHandle(ObjectHandle::kSamplerState,
      name 
        ? _sampler_states.Insert(name, ss)
        : _sampler_states.Insert(ss));
  }
  return emptyGoh;
}

//------------------------------------------------------------------------------
ObjectHandle Graphics::CreateSwapChain(
    const TCHAR* name,
    u32 width,
    u32 height,
    DXGI_FORMAT format,
    WNDPROC wndProc,
    HINSTANCE instance)
{
  // Register the window class
  WNDCLASSEX wcex;
  ZeroMemory(&wcex, sizeof(wcex));
  wcex.cbSize         = sizeof(WNDCLASSEX);
  wcex.style          = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc    = wndProc;
  wcex.hInstance      = instance;
  wcex.hbrBackground  = 0;
  wcex.lpszClassName  = name;

  if (!RegisterClassEx(&wcex))
  {
    return emptyGoh;
  }

  const UINT windowStyle = WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

  // Create/resize the window
  HWND hwnd = CreateWindow(name, g_AppWindowTitle, windowStyle,
    CW_USEDEFAULT, CW_USEDEFAULT, width, height, NULL, NULL,
    instance, NULL);

  SetClientSize(hwnd, width, height);
  ShowWindow(hwnd, SW_SHOW);

  // Create the swap chain
  DXGI_SWAP_CHAIN_DESC swapChainDesc;
  IDXGISwapChain* sc = 0;
  ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
  swapChainDesc.BufferCount            = 2;
  swapChainDesc.BufferDesc.Width       = width;
  swapChainDesc.BufferDesc.Height      = height;
  swapChainDesc.BufferDesc.Format      = format;
  swapChainDesc.BufferDesc.RefreshRate = selectedDisplayMode().RefreshRate;
  swapChainDesc.BufferUsage            = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  swapChainDesc.OutputWindow           = hwnd;
  swapChainDesc.SampleDesc.Count       = _curSetup.multisampleCount;
  swapChainDesc.SampleDesc.Quality     = 0;
  swapChainDesc.Windowed               = _curSetup.windowed;

  if (FAILED(_curSetup.dxgi_factory->CreateSwapChain(_device, &swapChainDesc, &sc)))
  {
    return emptyGoh;
  }

  SwapChain* swapChain = new SwapChain(name);
  swapChain->_hwnd = hwnd;
  swapChain->_desc = swapChainDesc;
  swapChain->_swapChain = sc;
  if (swapChain->CreateBackBuffers(width, height, format))
  {
    return ObjectHandle(
      ObjectHandle::kSwapChain, _swapChains.Insert(swapChain));
  }
  return emptyGoh;
}

//------------------------------------------------------------------------------
ObjectHandle Graphics::FindResource(const string &name)
{
  if (name.empty())
    return _dummyTexture;

  // check textures, then resources, then render targets
  int idx = _textures.IndexFromKey(name);
  if (idx != -1)
    return ObjectHandle(ObjectHandle::kTexture, idx);

  idx = _resources.IndexFromKey(name);
  if (idx != -1)
    return ObjectHandle(ObjectHandle::kResource, idx);

  idx = _renderTargets.IndexFromKey(name);
  return MakeObjectHandle(ObjectHandle::kRenderTarget, idx);
}

//------------------------------------------------------------------------------
ObjectHandle Graphics::FindSampler(const string &name)
{
  return MakeObjectHandle(ObjectHandle::kSamplerState, _sampler_states.IndexFromKey(name));
}

//------------------------------------------------------------------------------
ObjectHandle Graphics::FindBlendState(const string &name)
{
  return MakeObjectHandle(ObjectHandle::kBlendState, _blendStates.IndexFromKey(name));
}

//------------------------------------------------------------------------------
ObjectHandle Graphics::FindRasterizerState(const string &name)
{
  return MakeObjectHandle(ObjectHandle::kRasterizerState, _rasterizerStates.IndexFromKey(name));
}

//------------------------------------------------------------------------------
ObjectHandle Graphics::FindDepthStencilState(const string &name)
{
  return MakeObjectHandle(ObjectHandle::kDepthStencilState, _depthStencilStates.IndexFromKey(name));
}

//------------------------------------------------------------------------------
Graphics::SwapChain* Graphics::GetSwapChain(ObjectHandle h)
{
  return _swapChains.Get(h);
}

//------------------------------------------------------------------------------
ObjectHandle Graphics::MakeObjectHandle(
    ObjectHandle::Type type,
    int idx,
    int data)
{
  return idx != -1 ? ObjectHandle(type, idx, data) : emptyGoh;
}

//------------------------------------------------------------------------------
void Graphics::DestroyDeferredContext(DeferredContext *ctx)
{
  if (!ctx)
    return;
  if (!ctx->_is_immediate_context)
    ctx->_ctx->Release();
  delete exch_null(ctx);
}

//------------------------------------------------------------------------------
DeferredContext* Graphics::CreateDeferredContext(bool canUseImmediate)
{
  DeferredContext *dc = new DeferredContext;
  if (canUseImmediate)
  {
    dc->_is_immediate_context = true;
    dc->_ctx = _immediateContext;
  } else {
    _device->CreateDeferredContext(0, &dc->_ctx);
  }
  return dc;
}

//------------------------------------------------------------------------------
void Graphics::GetPredefinedGeometry(
    PredefinedGeometry geom,
    ObjectHandle *vb,
    int *vertex_size,
    ObjectHandle *ib, 
    DXGI_FORMAT *index_format,
    int *index_count)
{
  *index_format = DXGI_FORMAT_R16_UINT;
  assert(_predefinedGeometry.find(geom) != _predefinedGeometry.end());

  switch (geom)
  {
    case kGeomFsQuadPos:
      *vb = _predefinedGeometry[kGeomFsQuadPos].first;
      *ib = _predefinedGeometry[kGeomFsQuadPos].second;
      *vertex_size = sizeof(Vector4);
      *index_count = 6;
      break;

    case kGeomFsQuadPosTex:
      *vb = _predefinedGeometry[kGeomFsQuadPosTex].first;
      *ib = _predefinedGeometry[kGeomFsQuadPosTex].second;
      *vertex_size = sizeof(Pos4Tex);
      *index_count = 6;
      break;
  }
}

//------------------------------------------------------------------------------
void Graphics::AddCommandList(ID3D11CommandList *cmd_list)
{
  _immediateContext->ExecuteCommandList(cmd_list, FALSE);
  cmd_list->Release();
}

//------------------------------------------------------------------------------
void Graphics::CreateDefaultSwapChain(
    u32 width,
    u32 height,
    DXGI_FORMAT format,
    WNDPROC wndProc,
    HINSTANCE instance)
{
  _swapChain = CreateSwapChain(_T("default"), width, height, format, wndProc, instance);
}

//------------------------------------------------------------------------------
void Graphics::Present()
{
#if WITH_FONT_RENDERING
  for (const TextElement& text : _textElements)
  {
    _fw1FontWrapper->DrawString(
        _immediateContext, text.str.c_str(), text.size, text.x, text.y, text.color, FW1_RESTORESTATE);
  }

  _textElements.clear();
#endif

  _swapChains.Get(_swapChain)->Present();
}

//------------------------------------------------------------------------------
void Graphics::GetBackBufferSize(int* width, int* height)
{
  const Graphics::SwapChain* swapChain = _swapChains.Get(_swapChain);
  *width = (int)swapChain->_viewport.Width;
  *height = (int)swapChain->_viewport.Height;
}

//------------------------------------------------------------------------------
ObjectHandle Graphics::DefaultSwapChain()
{
  return _swapChain;
}

//------------------------------------------------------------------------------
void Graphics::AddText(
    const char* text,
    const char* font,
    float size,
    float x,
    float y,
    u32 color)
{
#if WITH_FONT_RENDERING
  _textElements.emplace_back(utf8_to_wide(text), _fw1FontWrapper, size, x, y, color);
#endif
}

//------------------------------------------------------------------------------
void Graphics::AddText(
    const wchar_t* text,
    const char* font,
    float size,
    float x,
    float y,
    u32 color)
{
#if WITH_FONT_RENDERING
  _textElements.emplace_back(text, _fw1FontWrapper, size, x, y, color);
#endif
}

//------------------------------------------------------------------------------
ObjectHandle Graphics::RenderTargetForSwapChain(ObjectHandle h)
{
  if (!h.IsValid())
    return ObjectHandle();

  auto swapChain = GRAPHICS._swapChains.Get(h);
  return swapChain->_renderTarget;
}

//------------------------------------------------------------------------------
bool Graphics::LoadComputeShadersFromFile(
    const string& filenameBase,
    ObjectHandle* shader,
    const char* entry)
{
#if WITH_DEBUG_SHADERS
  string suffix = ToString("_%sD.cso", entry);
#else
  string suffix = ToString("_%s.cso", entry);
#endif

  bool res;
  RESOURCE_MANAGER.AddFileWatch((filenameBase + suffix).c_str(), nullptr, true, &res,
    [=](const string& filename, void* token)
  {
    vector<char> buf;
    if (!RESOURCE_MANAGER.LoadFile(filename.c_str(), &buf))
    {
      LOG_WARN("Unable to load shader" << LogKeyValue("filename", filename));
      return false;
    }

    *shader = GRAPHICS.CreateComputeShader(buf, entry);
    if (!shader->IsValid())
    {
      LOG_WARN("Unable to create shader" << LogKeyValue("filename", filename));
      return false;
    }

    return true;
  });

  if (!res)
    return false;

  return true;
}

//------------------------------------------------------------------------------
bool Graphics::LoadShadersFromFile(
    const string& filenameBase,
    ObjectHandle* vs,
    ObjectHandle* ps,
    ObjectHandle* inputLayout,
    u32 vertexFlags,
    const char* vsEntry,
    const char* psEntry)
{
#if WITH_DEBUG_SHADERS
  string vsSuffix = ToString("_%sD.vso", vsEntry);
  string psSuffix = ToString("_%sD.pso", psEntry);
#else
  string vsSuffix = ToString("_%s.vso", vsEntry);
  string psSuffix = ToString("_%s.pso", psEntry);
#endif

  if (vs)
  {
    // todo: preallocate the shader handle to avoid writing to a pointer like this..
    bool res;
    RESOURCE_MANAGER.AddFileWatch((filenameBase + vsSuffix).c_str(), nullptr, true, &res,
      [=](const string& filename, void* token)
      {
        vector<char> buf;

        if (!RESOURCE_MANAGER.LoadFile(filename.c_str(), &buf))
        {
          LOG_WARN("Unable to load vertex shader" << LogKeyValue("filename", filename));
          return false;
        }

        ObjectHandle h = GRAPHICS.CreateVertexShader(buf, vsEntry);
        if (!h.IsValid())
        {
          LOG_WARN("Unable to create vertex shader" << LogKeyValue("filename", filename));
          return false;
        }

        *vs = h;

        if (inputLayout)
        {
          vector<D3D11_INPUT_ELEMENT_DESC> desc;
          u32 ofs = 0;
          if (vertexFlags & VF_POS)
          {
            D3D11_INPUT_ELEMENT_DESC element =
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
            desc.push_back(element);
            ofs += 12;
          }

          if (vertexFlags & VF_NORMAL)
          {
            D3D11_INPUT_ELEMENT_DESC element =
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, ofs, D3D11_INPUT_PER_VERTEX_DATA, 0 };
            desc.push_back(element);
            ofs += 12;
          }

          if (vertexFlags & VF_COLOR)
          {
            D3D11_INPUT_ELEMENT_DESC element =
            { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, ofs, D3D11_INPUT_PER_VERTEX_DATA, 0 };
            desc.push_back(element);
            ofs += 16;
          }

          *inputLayout = GRAPHICS.CreateInputLayout(desc, buf);
          if (!inputLayout->IsValid())
            return false;
        }
        return true;
      });

    if (!res)
      return false;
  }

  if (ps)
  {
    bool res;
    RESOURCE_MANAGER.AddFileWatch((filenameBase + psSuffix).c_str(), nullptr, true, &res,
      [=](const string& filename, void* token)
    {
      vector<char> buf;
      if (!RESOURCE_MANAGER.LoadFile(filename.c_str(), &buf))
      {
        LOG_WARN("Unable to load pixel shader" << LogKeyValue("filename", filename));
        return false;
      }

      *ps = GRAPHICS.CreatePixelShader(buf, psEntry);
      if (!ps->IsValid())
      {
        LOG_WARN("Unable to create pixel shader" << LogKeyValue("filename", filename));
        return false;
      }

      return true;
    });

    if (!res)
      return false;
  }

  return true;
}
