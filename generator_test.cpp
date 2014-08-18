#include "generator_test.hpp"
#include "graphics.hpp"
#include "deferred_context.hpp"
#include "app.hpp"
#include "demo_engine.hpp"
#include "proto_helpers.hpp"
#include "resource_manager.hpp"

#include "boba_loader.hpp"
#include "protocol/generator_bindings.hpp"
#include "debug_drawer.hpp"
#include "scene.hpp"
#include "dynamic_mesh.hpp"

#pragma warning(push)
#pragma warning(disable: 4244 4267)
#include "protocol/effect_settings_plexus.pb.h"
#pragma warning(pop)


using namespace boba;
using namespace bristol;

DynamicMesh g_mesh;

struct TextWriter
{
  bool Init(const char* filename)
  {
    if (!_loader.Load(filename))
    {
      return false;
    }

    u32 numLetters = 'Z' - 'A' + 1;
    _letters.resize(numLetters);

    Letter* curLetter = nullptr;

    for (u32 i = 0; i < _loader.meshes.size(); ++i)
    {
      // Letter mesh names are ['A'..'Z']
      BobaLoader::MeshElement* e = _loader.meshes[i];
      int t = (int)e->name[0] - 'A';
      if (strlen(e->name) == 1 && t >= 0 && t < (int)numLetters)
      {
        curLetter = &_letters[t];
        curLetter->outline = e;
      }
      else if (strcmp(e->name, "Cap 1") == 0)
      {
        if (curLetter)
          curLetter->cap1 = e;
        else
          LOG_WARN("Cap found without matching letter!");
      }
      else if (strcmp(e->name, "Cap 2") == 0)
      {
        if (curLetter)
          curLetter->cap2 = e;
        else
          LOG_WARN("Cap found without matching letter!");
      }
    }

    GRAPHICS.LoadShadersFromFile("shaders/text_shader",
      &_meshObjects._vs, &_meshObjects._ps, &_meshObjects._layout, VF_POS | VF_NORMAL | VF_COLOR);

    return true;
  }

  void AppendMeshToBuffer(
    const BobaLoader::MeshElement* e, 
    float xOfs, float yOfs, float zOfs,
    const Color& color,
    vector<float>* verts,
    vector<u32>* indices,
    float* minX,
    float* maxX)
  {
    u32 numVerts = (u32)verts->size();
    u32 numIndices = (u32)indices->size();

    u32 v = e->numVerts / 3;
    u32 stride = 10;
    verts->resize(verts->size() + v * stride);
    indices->resize(indices->size() + e->numIndices);

    *minX = *maxX = e->verts[0];
    for (u32 i = 0; i < e->numVerts/3; ++i) {
      float x = e->verts[i*3+0];
      float y = e->verts[i*3+1];
      float z = e->verts[i*3+2];

      (*verts)[numVerts+i*stride+0] = x + xOfs;
      (*verts)[numVerts+i*stride+1] = y + yOfs;
      (*verts)[numVerts+i*stride+2] = z + zOfs;

      (*verts)[numVerts+i*stride+3] = e->normals[i*3+0];
      (*verts)[numVerts+i*stride+4] = e->normals[i*3+1];
      (*verts)[numVerts+i*stride+5] = e->normals[i*3+2];

      (*verts)[numVerts+i*stride+6] = color.x;
      (*verts)[numVerts+i*stride+7] = color.y;
      (*verts)[numVerts+i*stride+8] = color.z;
      (*verts)[numVerts+i*stride+9] = color.w;

      *minX = min(*minX, x);
      *maxX = max(*maxX, x);
    }

    // update indices to point to the correct spot in the combined VB
    u32 ofs = numVerts / 10;
    for (u32 i = 0; i < e->numIndices; ++i)
    {
      (*indices)[numIndices+i] = e->indices[i] + ofs;
    }
  }

  void WriteText(const char* text)
  {
    u32 len = (u32)strlen(text);

    vector<float> verts;
    vector<u32> indices;

    float xOfs = -1000;
    float yOfs = -80;
    float zOfs = 1000;

    for (u32 i = 0; i < len; ++i)
    {
      if (text[i] == ' ')
      {
        xOfs += 50;
        continue;
      }

      int t = toupper(text[i]) - 'A';
      Letter* letter = &_letters[t];
      BobaLoader::MeshElement* e = letter->outline;

      float minX, maxX;
      AppendMeshToBuffer(e, xOfs, yOfs, zOfs, Color(1,0,0,0), &verts, &indices, &minX, &maxX);
      AppendMeshToBuffer(letter->cap1, xOfs, yOfs, zOfs, Color(0,1,0,0), &verts, &indices, &minX, &maxX);
      //AppendMeshToBuffer(letter->cap2, xOfs, yOfs, zOfs, &verts, &indices, &minX, &maxX);

      xOfs += 1.05f * (maxX - minX);

    }

    _numVerts = (u32)verts.size() / 10;
    _numIndices = (u32)indices.size();

    _meshObjects.CreateDynamic(
      _numIndices * sizeof(u32), DXGI_FORMAT_R32_UINT, indices.data(),
      _numVerts * sizeof(PosNormalColor), sizeof(PosNormalColor), verts.data());
  }

  GpuObjects _meshObjects;

  u32 _numVerts;
  u32 _numIndices;

  struct Letter
  {
    Letter() { memset(this, 0, sizeof(Letter)); }
    BobaLoader::MeshElement* outline;
    BobaLoader::MeshElement* cap1;
    BobaLoader::MeshElement* cap2;
  };

  vector<Letter> _letters;
  BobaLoader _loader;
};

TextWriter g_textWriter;

//------------------------------------------------------------------------------
GeneratorTest::GeneratorTest(const string& name, u32 id)
    : Effect(name, id)
    , _rotatingObject(false)
    , _dirtyFlag(true)
    , _lua(nullptr)
    , _numIndices(0)
    , _cameraDir(0,0,1)
    , _curAdaption(0)
{
  _renderFlags.Set(GeneratorTest::RenderFlags::Luminance);
}

//------------------------------------------------------------------------------
GeneratorTest::~GeneratorTest()
{
}

//------------------------------------------------------------------------------
bool GeneratorTest::Init(const char* config)
{
  _configName = config;

  if (!LoadProto(config, &_planeConfig))
  {
    LOG_ERROR(ToString("Error loading config: %s", config).c_str());
    return false;
  }

  if (_planeConfig.has_camera_pos()) _cameraPos = ::FromProtocol(_planeConfig.camera_pos());
  if (_planeConfig.has_camera_dir()) _cameraDir = ::FromProtocol(_planeConfig.camera_dir());
  if (_planeConfig.has_obj_t()) g_mesh.translation = ::FromProtocol(_planeConfig.obj_t());
  if (_planeConfig.has_obj_r()) g_mesh.rotation = ::FromProtocol(_planeConfig.obj_r());
  _prevRot = g_mesh.rotation;

  //BindSpiky(&_spikyConfig, &_dirtyFlag);
  static bool tmp;
  BindPlane(&_planeConfig, &tmp);

  _meshObjects.CreateDynamic(64 * 1024, DXGI_FORMAT_R32_UINT, 64 * 1024, sizeof(PosNormal));
  _cb.Create();

  _cbToneMapping.Create();
  _luminanceAdaption[0] = GRAPHICS.CreateRenderTarget(1, 1, DXGI_FORMAT_R16_FLOAT, BufferFlags(BufferFlag::CreateSrv));
  _luminanceAdaption[1] = GRAPHICS.CreateRenderTarget(1, 1, DXGI_FORMAT_R16_FLOAT, BufferFlags(BufferFlag::CreateSrv));

  GRAPHICS.LoadShadersFromFile("shaders/generator",
      &_meshObjects._vs, &_meshObjects._ps, &_meshObjects._layout, VF_POS | VF_NORMAL);

  InitGeneratorScript();

  if (!DebugDrawer::Create())
    return false;

  g_mesh.Init();

  _postProcess = make_unique<PostProcess>(_ctx);
  if (!_postProcess->Init())
    return false;

  int w, h;
  GRAPHICS.GetBackBufferSize(&w, &h);
  _renderTarget = GRAPHICS.CreateRenderTarget(w, h, DXGI_FORMAT_R16G16B16A16_FLOAT,
      BufferFlags(BufferFlag::CreateDepthBuffer) | BufferFlag::CreateSrv);
  
  if (!GRAPHICS.LoadShadersFromFile("shaders/copy", nullptr, &_psCopy, nullptr, 0))
    return false;

  if (!GRAPHICS.LoadShadersFromFile("shaders/tonemap", nullptr, &_psLuminance, nullptr, 0, nullptr, "LuminanceMap"))
    return false;

  if (!GRAPHICS.LoadShadersFromFile("shaders/tonemap", nullptr, &_psComposite, nullptr, 0, nullptr, "Composite"))
    return false;

  if (!GRAPHICS.LoadShadersFromFile("shaders/tonemap", nullptr, &_psAdaption, nullptr, 0, nullptr, "AdaptLuminance"))
    return false;

  if (!GRAPHICS.LoadShadersFromFile("shaders/tonemap", nullptr, &_psThreshold, nullptr, 0, nullptr, "BloomThreshold"))
    return false;

  if (!GRAPHICS.LoadComputeShadersFromFile("shaders/blur", &_csBlurX, "BoxBlurX"))
    return false;

  if (!GRAPHICS.LoadComputeShadersFromFile("shaders/blur", &_csBlurY, "BoxBlurY"))
    return false;

  if (!GRAPHICS.LoadComputeShadersFromFile("shaders/blur", &_csCopyTranspose, "CopyTranspose"))
    return false;

  if (!GRAPHICS.LoadComputeShadersFromFile("shaders/blur", &_csBlurTranspose, "BlurTranspose"))
    return false;

  if (!GRAPHICS.LoadShadersFromFile("shaders/text_shader", nullptr, &_psEdgeDetect, nullptr, 0, nullptr, "EdgeDetect"))
    return false;


  _cbBlur.Create();
  _cbBloom.Create();
  _cbComposite.Create();

  _depthStencilState = GRAPHICS.CreateDepthStencilState(CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT()));
  _blendState = GRAPHICS.CreateBlendState(CD3D11_BLEND_DESC(CD3D11_DEFAULT()));
  _rasterizerState = GRAPHICS.CreateRasterizerState(CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT()));

  g_textWriter.Init("meshes/text1.boba");
  g_textWriter.WriteText("NEUROTICA EFS");

  return true;
}

//------------------------------------------------------------------------------
void GeneratorTest::InitGeneratorScript()
{
  SetDynamicMesh(&g_mesh);
  RESOURCE_MANAGER.AddFileWatch("scripts/generator2.lua", 0, true, nullptr,
    [this](const string& filename, void* token)
  {
    if (_lua)
      lua_close(_lua);

    _lua = luaL_newstate();
    luaL_openlibs(_lua);
    luaL_requiref(_lua, "meshlib", luaopen_meshlib, 1);
    luaL_requiref(_lua, "mesh", luaopen_mesh, 1);

    if (luaL_loadfile(_lua, filename.c_str()))
    {
      APP.AddMessage(MessageType::Error, "Error loading " + filename);
      return false;
    }
    else
    {
      APP.AddMessage(MessageType::Info, "Loaded " + filename);
    }

    lua_pcall(_lua, 0, 0, 0);
    _dirtyFlag = true;
    return true;
  });
}

//------------------------------------------------------------------------------
bool GeneratorTest::Update(const UpdateState& state)
{
  _updateState = state;
  return true;
}

//------------------------------------------------------------------------------
void GeneratorTest::RenderPlane()
{
  if (_dirtyFlag)
  {
    g_mesh.Reset();

    // push debug.traceback on stack 
    lua_getglobal(_lua, "debug");
    lua_getfield(_lua, -1, "traceback");
    lua_replace(_lua, -2);

    lua_getglobal(_lua, "generate");
    lua_pushnumber(_lua, _planeConfig.width());
    lua_pushnumber(_lua, _planeConfig.height());
    lua_pushinteger(_lua, _planeConfig.width_segments());
    lua_pushinteger(_lua, _planeConfig.height_segments());

    if (lua_pcall(_lua, 4, 0, -6))
    {
      const char* err = lua_tostring(_lua, -1);
      OutputDebugStringA(err);
      OutputDebugStringA("\n");
      APP.AddMessage(MessageType::Warning, err);
      lua_pop(_lua, 2);
    }
    else
    {
      // check if the buffers need to be resized
      u32 vbSize = (u32)(g_mesh.verts.size() * sizeof(g_mesh.verts[0]));
      if (vbSize > _meshObjects._vbSize)
      {
        while (_meshObjects._vbSize < vbSize)
          _meshObjects._vbSize <<= 1;
        _meshObjects._vb = GRAPHICS.CreateBuffer(D3D11_BIND_VERTEX_BUFFER, _meshObjects._vbSize, true, nullptr, sizeof(PosNormal));
      }

      u32 ibSize = (u32)(g_mesh.indices.size() * sizeof(u32));
      if (ibSize > _meshObjects._ibSize)
      {
        while (_meshObjects._ibSize < ibSize)
          _meshObjects._ibSize <<= 1;
        _meshObjects._ib = GRAPHICS.CreateBuffer(D3D11_BIND_INDEX_BUFFER, _meshObjects._ibSize, true, nullptr, DXGI_FORMAT_R32_UINT);
      }

      g_mesh.CalcBoundingSphere();

      _ctx->CopyToBuffer(_meshObjects._vb, 0, D3D11_MAP_WRITE_DISCARD, 0, g_mesh.verts.data(), vbSize);
      _ctx->CopyToBuffer(_meshObjects._ib, 0, D3D11_MAP_WRITE_DISCARD, 0, g_mesh.indices.data(), ibSize);

      _numIndices = (u32)g_mesh.indices.size();
    }
    lua_pop(_lua, 1);

    _dirtyFlag = false;
  }

}

//------------------------------------------------------------------------------
void GeneratorTest::RenderSpiky()
{
  if (_dirtyFlag)
  {
    g_mesh.Reset();

    // push debug.traceback on stack 
    lua_getglobal(_lua, "debug");
    lua_getfield(_lua, -1, "traceback");
    lua_replace(_lua, -2);

    lua_getglobal(_lua, "generate");
    lua_pushnumber(_lua, _spikyConfig.radius());
    lua_pushnumber(_lua, _spikyConfig.height());
    lua_pushinteger(_lua, _spikyConfig.radial_segments());
    lua_pushinteger(_lua, _spikyConfig.height_segments());

    if (lua_pcall(_lua, 4, 0, -6))
    {
      const char* err = lua_tostring(_lua, -1);
      OutputDebugStringA(err);
      OutputDebugStringA("\n");
      APP.AddMessage(MessageType::Warning, err);
      lua_pop(_lua, 2);
    }
    else
    {
      // check if the buffers need to be resized
      u32 vbSize = (u32)g_mesh.verts.size() * sizeof(g_mesh.verts[0]);
      if (vbSize > _meshObjects._vbSize)
      {
        while (_meshObjects._vbSize < vbSize)
          _meshObjects._vbSize <<= 1;
        _meshObjects._vb = GRAPHICS.CreateBuffer(D3D11_BIND_VERTEX_BUFFER, _meshObjects._vbSize, true, nullptr, sizeof(PosNormal));
      }

      u32 ibSize = (u32)g_mesh.indices.size() * sizeof(u32);
      if (ibSize > _meshObjects._ibSize)
      {
        while (_meshObjects._ibSize < ibSize)
          _meshObjects._ibSize <<= 1;
        _meshObjects._ib = GRAPHICS.CreateBuffer(D3D11_BIND_INDEX_BUFFER, _meshObjects._ibSize, true, nullptr, DXGI_FORMAT_R32_UINT);
      }

      g_mesh.CalcBoundingSphere();

      D3D11_MAPPED_SUBRESOURCE res;
      if (_ctx->Map(_meshObjects._vb, 0, D3D11_MAP_WRITE_DISCARD, 0, &res))
      {
        memcpy(res.pData, g_mesh.verts.data(), vbSize);
        _ctx->Unmap(_meshObjects._vb, 0);
      }

      if (_ctx->Map(_meshObjects._ib, 0, D3D11_MAP_WRITE_DISCARD, 0, &res))
      {
        memcpy(res.pData, g_mesh.indices.data(), ibSize);
        _ctx->Unmap(_meshObjects._ib, 0);
      }

      _numIndices = (u32)g_mesh.indices.size();
    }
    lua_pop(_lua, 1);

    _dirtyFlag = false;
  }
}

//------------------------------------------------------------------------------
void GeneratorTest::RenderText()
{
  Color black(0.1f, 0.1f, 0.1f, 0);
  float blendFactor[4] ={ 1, 1, 1, 1 };
  _ctx->SetRasterizerState(_rasterizerState);
  _ctx->SetBlendState(_blendState, blendFactor, 0xffffffff);
  _ctx->SetDepthStencilState(_depthStencilState, 0);

  int w, h;
  GRAPHICS.GetBackBufferSize(&w, &h);

  auto f = BufferFlags(BufferFlag::CreateSrv) | BufferFlags(BufferFlag::CreateDepthBuffer);
  ScopedRenderTarget scratch(w, h, DXGI_FORMAT_R8G8B8A8_UNORM, f);
  _ctx->SetRenderTarget(scratch.h, &black);

  _ctx->BeginFrame();

  _proj = Matrix::CreatePerspectiveFieldOfView(45.0f / 180 * 3.1415f, 16.0f / 10, 1, 10000);
  _view = Matrix::CreateLookAt(_cameraPos, _cameraPos + 100 * _cameraDir, Vector3(0, 1, 0));
  _invView = _view.Invert();

  Matrix world = Matrix::Identity();
  world = g_mesh.rotation;

  _cb.data.world = world.Transpose();
  _cb.data.viewProj = (_view * _proj).Transpose();

  GPU_BeginEvent(0xffffffff, L"DrawObj");

  _ctx->SetRasterizerState(_renderFlags.IsSet(RenderFlags::Wireframe) ? g_mesh._wireframe : g_mesh._solid);
  _ctx->SetCBuffer(_cb, ShaderType::VertexShader, 0);

  _ctx->SetRenderObjects(g_textWriter._meshObjects);
  _ctx->DrawIndexed(g_textWriter._numIndices, 0, 0);
  GPU_EndEvent();

  _ctx->SetVB(nullptr, 24);
  _ctx->SetIB(nullptr, DXGI_FORMAT_R32_UINT);
  _ctx->UnsetSRVs(0, 1, ShaderType::PixelShader);

  GraphicsObjectHandle rtDest = GRAPHICS.RenderTargetForSwapChain(GRAPHICS.DefaultSwapChain());

  _postProcess->Setup();
  _postProcess->Execute({ scratch.h }, rtDest, _psEdgeDetect, &black, L"EdgeDetect");

  _ctx->EndFrame();
  _ctx->SetSwapChain(GRAPHICS.DefaultSwapChain(), false);

}

//------------------------------------------------------------------------------
bool GeneratorTest::Render()
{
  RenderText();
  return true;

  RenderPlane();

  Color black(0,0,0,0);
  float blendFactor[4] ={ 1, 1, 1, 1 };

  _ctx->SetRasterizerState(_rasterizerState);
  _ctx->SetBlendState(_blendState, blendFactor, 0xffffffff);
  _ctx->SetDepthStencilState(_depthStencilState, 0);
  _ctx->SetRenderTarget(_renderTarget, &black);

  if (_numIndices >= 3)
  {
    _ctx->BeginFrame();

    _proj = Matrix::CreatePerspectiveFieldOfView(45.0f / 180 * 3.1415f, 16.0f / 10, 1, 1000);
    _view = Matrix::CreateLookAt(_cameraPos, _cameraPos + 100 * _cameraDir, Vector3(0, 1, 0));
    _invView = _view.Invert();

    Matrix world = g_mesh.World();
    _cb.data.world = world.Transpose();
    _cb.data.viewProj = (_view * _proj).Transpose();

    GPU_BeginEvent(0xffffffff, L"DrawObj");

    _ctx->SetRasterizerState(_renderFlags.IsSet(RenderFlags::Wireframe) ? g_mesh._wireframe : g_mesh._solid);
    _ctx->SetCBuffer(_cb, ShaderType::VertexShader, 0);

    _ctx->SetRenderObjects(_meshObjects);
    _ctx->DrawIndexed(_numIndices, 0, 0);

    GPU_EndEvent();

    ScopedRenderTarget rtLuminance(1024, 1024, DXGI_FORMAT_R32_FLOAT,
        BufferFlags(BufferFlag::CreateSrv) | BufferFlag::CreateMipMaps);

    // we want the geometric mean of the luminance, which can be calculated as
    // exp(avg(log(x)), where the avg(log) term can be obtained as the lowest mip level
    // from a texture containing the log luminance
    _postProcess->Setup();
    _postProcess->Execute({_renderTarget}, rtLuminance.h, _psLuminance, &black, L"Luminance");
    _ctx->GenerateMips(rtLuminance.h);

    // luminance level adaption
    _cbToneMapping.data.tau = _planeConfig.tau();
    _cbToneMapping.data.key = _planeConfig.key();
    _cbToneMapping.data.ofs = _planeConfig.ofs();
    _cbToneMapping.data.delta = _updateState.delta.TotalMicroseconds() / 1e6f;
    _ctx->SetCBuffer(_cbToneMapping, ShaderType::PixelShader, 1);

    // Adaption will perform the exp() on the average luminance
    _postProcess->Execute({_luminanceAdaption[!_curAdaption], rtLuminance.h },
        _luminanceAdaption[_curAdaption], _psAdaption, &black, L"Adaption");

    int w, h;
    GRAPHICS.GetBackBufferSize(&w, &h);

    // bloom threshold
    ScopedRenderTarget threshold(w, h, DXGI_FORMAT_R16G16B16A16_FLOAT, BufferFlags(BufferFlag::CreateSrv));
    _cbBloom.data.threshold = _planeConfig.bloom_threshold();
    _ctx->SetCBuffer(_cbBloom, ShaderType::PixelShader, 2);
    _postProcess->Execute({ _renderTarget, _luminanceAdaption[_curAdaption] }, threshold.h, _psThreshold, &black, L"BloomThreshold");

    auto f = BufferFlags(BufferFlag::CreateSrv) | BufferFlag::CreateUav;
    ScopedRenderTarget blur0(w, h, DXGI_FORMAT_R16G16B16A16_FLOAT, f);
    ScopedRenderTarget blur1(w, h, DXGI_FORMAT_R16G16B16A16_FLOAT, f);

    // scratch buffer for the blur
    int s = max(w, h);
    ScopedRenderTarget scratch0(s, s, DXGI_FORMAT_R16G16B16A16_FLOAT, f);
    ScopedRenderTarget scratch1(s, s, DXGI_FORMAT_R16G16B16A16_FLOAT, f);

    // set constant buffers
    _cbBlur.data.inputSize.x = (float)w;
    _cbBlur.data.inputSize.y = (float)h;
    _cbBlur.data.radius = _planeConfig.blur_radius();
    _ctx->SetCBuffer(_cbBlur, ShaderType::ComputeShader, 0);

    if (_planeConfig.transpose())
    {
      // set constant buffers
      GraphicsObjectHandle srcDst[] =
      {
        // horiz
        threshold.h, scratch0.h, scratch0.h, scratch1.h, scratch1.h, scratch0.h,
        // vert
        scratch1.h, scratch0.h, scratch0.h, scratch1.h, scratch1.h, scratch0.h,
      };

      GPU_BeginEvent(0xffffffff, L"blurX_T");
      // horizontal blur (ends up in scratch0)
      for (int i = 0; i < 3; ++i)
      {
        _ctx->SetShaderResources({ srcDst[i*2+0] }, ShaderType::ComputeShader);
        _ctx->SetUav(srcDst[i*2+1], &black);

        _ctx->SetCS(_csBlurTranspose);
        _ctx->Dispatch(h/32+1, 1, 1);

        _ctx->UnsetUAVs(0, 1);
        _ctx->UnsetSRVs(0, 1, ShaderType::ComputeShader);

      }
      GPU_EndEvent();


      // copy/transpose from scratch0 -> scratch1
      GPU_BeginEvent(0xffffffff, L"CopyTranspose");
      _ctx->SetShaderResources({ scratch0.h }, ShaderType::ComputeShader);
      _ctx->SetUav(scratch1.h, &black);

      _ctx->SetCS(_csCopyTranspose);
      _ctx->Dispatch(h/32+1, 1, 1);

      _ctx->UnsetUAVs(0, 1);
      _ctx->UnsetSRVs(0, 1, ShaderType::ComputeShader);
      GPU_EndEvent();


      // "vertical" blur, ends up in scratch 0
      _cbBlur.data.inputSize.x = (float)h;
      _cbBlur.data.inputSize.y = (float)w;
      _cbBlur.data.radius = _planeConfig.blur_radius();
      _ctx->SetCBuffer(_cbBlur, ShaderType::ComputeShader, 0);

      GPU_BeginEvent(0xffffffff, L"blurY_T");
      for (int i = 0; i < 3; ++i)
      {

        _ctx->SetShaderResources({ srcDst[6+i*2+0] }, ShaderType::ComputeShader);
        _ctx->SetUav(srcDst[6+i*2+1], &black);

        _ctx->SetCS(_csBlurTranspose);
        _ctx->Dispatch(w/32+1, 1, 1);

        _ctx->UnsetUAVs(0, 1);
        _ctx->UnsetSRVs(0, 1, ShaderType::ComputeShader);
      }
      GPU_EndEvent();

      // copy/transpose from scratch0 -> blur1
      GPU_BeginEvent(0xffffffff, L"CopyTranspose");

      _ctx->SetShaderResources({ scratch0.h }, ShaderType::ComputeShader);
      _ctx->SetUav(blur1.h, &black);

      _ctx->SetCS(_csCopyTranspose);
      _ctx->Dispatch(w/32+1, 1, 1);

      _ctx->UnsetUAVs(0, 1);
      _ctx->UnsetSRVs(0, 1, ShaderType::ComputeShader);

      GPU_EndEvent();
    }
    else
    {
      GraphicsObjectHandle srcDst[] =
      {
        threshold.h, blur0.h, blur0.h, blur1.h,
        blur1.h, blur0.h, blur0.h, blur1.h,
        blur1.h, blur0.h, blur0.h, blur1.h,
      };

      for (int i = 0; i < 3; ++i)
      {
        GPU_BeginEvent(0xffffffff, L"blurX");

        _ctx->SetShaderResources({ srcDst[i*4+0] }, ShaderType::ComputeShader);
        _ctx->SetUav(srcDst[i*4+1], &black);

        _ctx->SetCS(_csBlurX);
        _ctx->Dispatch(h/32+1, 1, 1);

        _ctx->UnsetUAVs(0, 1);
        _ctx->UnsetSRVs(0, 1, ShaderType::ComputeShader);

        GPU_EndEvent();
        GPU_BeginEvent(0xffffffff, L"blurY");

        _ctx->SetShaderResources({ srcDst[i*4+2] }, ShaderType::ComputeShader);
        _ctx->SetUav(srcDst[i*4+3], &black);

        _ctx->SetCS(_csBlurY);
        _ctx->Dispatch(w/32+1, 1, 1);

        _ctx->UnsetUAVs(0, 1);
        _ctx->UnsetSRVs(0, 1, ShaderType::ComputeShader);

        GPU_EndEvent();
      }
    }

    // apply tone mapping and bloom
    GraphicsObjectHandle rtDest = GRAPHICS.RenderTargetForSwapChain(GRAPHICS.DefaultSwapChain());

    if (_renderFlags.IsSet(GeneratorTest::RenderFlags::Luminance))
    {
      _cbComposite.data.bloomMultiplier = _planeConfig.bloom_multiplier();
      _ctx->SetCBuffer(_cbComposite, ShaderType::PixelShader, 3);

      _postProcess->Execute({_renderTarget, _luminanceAdaption[_curAdaption], blur1.h},
          rtDest, _psComposite, &black, L"Composite");
    }
    else
    {
      _postProcess->Execute({blur1.h}, rtDest, _psCopy, &black, L"Copy");
    }

    _curAdaption = !_curAdaption;

    if (_renderFlags.IsSet(RenderFlags::Wireframe))
    {
      DEBUG_DRAW.SetContext(_ctx);
      DEBUG_DRAW.SetViewProjMatrix(_view, _proj);
      DEBUG_DRAW.SetWorldMatrix(world);
      DEBUG_DRAW.SetWidth(5);
      DEBUG_DRAW.DrawSphere(g_mesh.center, g_mesh.radius);
    }

    _ctx->EndFrame();
  }

  _ctx->SetSwapChain(GRAPHICS.DefaultSwapChain(), false);

  return true;
}

//------------------------------------------------------------------------------
bool GeneratorTest::Close()
{
  return true;
}

//------------------------------------------------------------------------------
bool GeneratorTest::SaveSettings()
{
  if (FILE* f = fopen(_configName.c_str() ,"wt"))
  {
    ::ToProtocol(_cameraPos, _planeConfig.mutable_camera_pos());
    ::ToProtocol(_cameraDir, _planeConfig.mutable_camera_dir());

    ::ToProtocol(g_mesh.translation, _planeConfig.mutable_obj_t());
    ::ToProtocol(g_mesh.rotation, _planeConfig.mutable_obj_r());

    fprintf(f, "%s", _planeConfig.DebugString().c_str());
    fclose(f);
  }
  return true;
}

//------------------------------------------------------------------------------
Effect* GeneratorTest::Create(const char* name, u32 id)
{
  return new GeneratorTest(name, id);
}

//------------------------------------------------------------------------------
const char* GeneratorTest::Name()
{
  return "generator_test";
}

//------------------------------------------------------------------------------
void GeneratorTest::ToProtocol(effect::protocol::EffectSetting* settings) const
{
  settings->set_type(effect::protocol::EffectSetting_Type_Plexus);

  effect::protocol::plexus::Plexus plexus;
  effect::protocol::plexus::TextPath* textPath = plexus.add_text_paths();
  textPath->set_text("neurotica");

  effect::protocol::plexus::NoiseEffector* effector = plexus.add_noise_effectors();
  effector->set_apply_to(effect::protocol::plexus::NoiseEffector_ApplyTo_Position);

  settings->set_config_msg(plexus.SerializeAsString());
}

//------------------------------------------------------------------------------
void GeneratorTest::FromProtocol(const std::string& str)
{
  effect::protocol::plexus::Plexus p;
  p.ParseFromString(str);
  int a = 10;
}

//------------------------------------------------------------------------------
void GeneratorTest::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
  DEBUG_DRAW.SetColor(Color(_rotatingObject * 1.0f, 1, 0, 1));
  float speed = 10;

  switch (message)
  {
    case WM_KEYDOWN:
      switch (wParam)
      {
        case '1':
        _renderFlags.Toggle(RenderFlags::Wireframe);
        break;

        case '2':
        _renderFlags.Toggle(RenderFlags::Luminance);
        break;

        case 'A':
        {
          _cameraPos += -speed * _invView.Right();
          break;
        }

        case 'D':
        {
          _cameraPos += speed * _invView.Right();
          break;
        }

        case 'W':
        {
          _cameraPos += speed * _invView.Up();
          break;
        }

        case 'S':
        {
          _cameraPos += -speed * _invView.Up();
          break;
        }

      }
    break;

    case WM_LBUTTONDOWN:
    {
      // start of rotation
      u32 xs = LOWORD(lParam);
      u32 ys = HIWORD(lParam);
      Vector3 org_vs = Vector3(0, 0, 0);
      Vector3 dir_vs = ScreenToViewSpace(_proj, xs, ys);
            
      Matrix worldToObj = g_mesh.World().Invert();
      Matrix viewToObj = _invView * worldToObj;

      Vector3 org_os = Vector4::Transform(Vector4(_invView._41,  _invView._42, _invView._43, 1), worldToObj);
      Vector3 dir_os = Vector4::Transform(Vector4(dir_vs.x, dir_vs.y, dir_vs.z, 0), viewToObj);
      dir_os.Normalize();
      float t = Raycast(Vector3(0, 0, 0), g_mesh.radius, org_os, dir_os);
      _rotatingObject = t > 0;

      _rotatingObject = true;
      if (_rotatingObject)
      {
        Vector3 d = Vector4::Transform(Vector4(dir_vs.x, dir_vs.y, dir_vs.z, 0), _invView);
        d.Normalize();
        _v0 = d;
      }

      break;
    }

    case WM_LBUTTONUP:
      _prevRot = g_mesh.rotation;
      _rotatingObject = false;
      break;

    case WM_MOUSEMOVE:
    {
      if (_rotatingObject)
      {
        u32 xs = LOWORD(lParam);
        u32 ys = HIWORD(lParam);

        // Find the click point in world space
        Vector3 pos = ScreenToViewSpace(_proj, xs, ys);

        // Vector from eye pos (0,0,0) to click point in world space
        Vector3 v1 = Vector4::Transform(Vector4(pos.x, pos.y, pos.z, 0), _invView);
        v1.Normalize();

        // angle between the two vectors
        float speed = 2;
        float angle = acos(_v0.Dot(v1));
        Vector3 axis = _v0.Cross(v1);
        if (axis.Length() > 0)
          g_mesh.rotation =  _prevRot * Matrix::CreateFromAxisAngle(axis, speed * angle);
      }
      break;
    }

    case WM_MOUSEWHEEL:
    {
      s16 delta = HIWORD(wParam);
      float fDelta = delta / 120.0f;
      _cameraPos += 10 * fDelta * _invView.Forward();
      break;
    }
    
  }
}

