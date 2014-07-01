#include "generator_test.hpp"
#include "graphics.hpp"
#include "deferred_context.hpp"
#include "app.hpp"
#include "demo_engine.hpp"
#include "proto_helpers.hpp"
#include "resource_manager.hpp"

#include "boba_io.hpp"
#include "protocol/generator_bindings.hpp"
#include "debug_drawer.hpp"

using namespace boba;
using namespace bristol;

struct Face
{
  Face() {}
  Face(u32 a, u32 b, u32 c) : a(a), b(b), c(c) {}
  u32 a, b, c;
  Vector3 normal;
};

struct Mesh
{
  Mesh()
  {
    Reset();
  }

  void Init()
  {
    CD3D11_RASTERIZER_DESC desc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT());
    desc.FillMode = D3D11_FILL_SOLID;
    _solid = GRAPHICS.CreateRasterizerState(desc);
    desc.FillMode = D3D11_FILL_WIREFRAME;
    desc.CullMode = D3D11_CULL_NONE;
    _wireframe = GRAPHICS.CreateRasterizerState(desc);
  }

  void Reset()
  {
    rotation = Matrix::Identity();
    translation = Vector3(0,0,0);

    indices.clear();
    verts.clear();
    faces.clear();
  }

  Matrix World()
  {
    return rotation * Matrix::CreateTranslation(translation);
  }

  void CalcBoundingSphere()
  {
    if (verts.empty())
      return;

    // find center
    size_t numVerts = verts.size() / 6;
    translation = Vector3(verts[0], verts[1], verts[2]);
    for (size_t i = 1*6; i < numVerts; ++i)
    {
      translation.x += verts[i * 6 + 0];
      translation.y += verts[i * 6 + 1];
      translation.z += verts[i * 6 + 2];
    }

    translation/= (float)numVerts;

    // center the verts
    for (size_t i = 0; i < numVerts; ++i)
    {
      verts[i * 6 + 0] -= translation.x;
      verts[i * 6 + 1] -= translation.y;
      verts[i * 6 + 2] -= translation.z;
    }

    // find max radius to center
    float r = Vector3::DistanceSquared(Vector3(0,0,0), Vector3(verts[0], verts[1], verts[2]));
    for (size_t i = 1*6; i < numVerts; ++i)
    {
      float cand = Vector3::DistanceSquared(Vector3(0,0,0), Vector3(verts[i * 6 + 0], verts[i * 6 + 1], verts[i * 6 + 2]));
      r = max(r, cand);
    }

    center = Vector3(0,0,0);
    radius = sqrt(r);
  }

  vector<float> verts;
  vector<u32> indices;
  vector<Face> faces;

  Vector3 center;
  float radius;

  Matrix rotation;
  Vector3 translation;

  GraphicsObjectHandle _wireframe;
  GraphicsObjectHandle _solid;
};

#define DEBUG_DRAW DebugDrawer::Instance()

Mesh g_mesh;

// vertex[index]
int VertexIndex(lua_State* L)
{
  // note, indices are 0 bases
  Mesh* mesh = *(Mesh**)luaL_checkudata(L, 1, "vertices");
  int index = luaL_checkint(L, 2);
  luaL_argcheck(L, index >= 0 && index < (int)mesh->verts.size(), 2, "index out of range");
  lua_pushnumber(L, mesh->verts[index]);
  return 1;
}

// metatable method for handle "vertex[index] = value"
int VertexNewIndex(lua_State* L)
{
  Mesh* mesh = *(Mesh**)luaL_checkudata(L, 1, "vertices");
  int index = luaL_checkint(L, 2);
  float value = luaL_checknumber(L, 3);
  if (index >= (int)mesh->verts.size())
    mesh->verts.resize(index+1);
  mesh->verts[index] = value;
  return 0;
}

// face[index]
int FaceIndex(lua_State* L)
{
  // note, indices are 0 bases
  Mesh* mesh = *(Mesh**)luaL_checkudata(L, 1, "indices");
  int index = luaL_checkint(L, 2);
  luaL_argcheck(L, index >= 0 && index < (int)mesh->indices.size(), 2, "index out of range");
  lua_pushinteger(L, mesh->indices[index]);
  return 1;
}

// metatable method for handle "face[index] = value"
int FaceNewIndex(lua_State* L)
{
  Mesh* mesh = *(Mesh**)luaL_checkudata(L, 1, "indices");
  int index = luaL_checkint(L, 2);
  int value = luaL_checkint(L, 3);
  if (index >= (int)mesh->indices.size())
    mesh->indices.resize(index+1);
  mesh->indices[index] = value;
  return 0;
}

int GetMeshVertices(lua_State* L)
{
  Mesh** mesh = (Mesh**)lua_newuserdata(L, sizeof(Mesh**));
  *mesh = &g_mesh;
  luaL_setmetatable(L, "vertices");
  return 1;
}

int GetMeshIndices(lua_State* L)
{
  Mesh** mesh = (Mesh**)lua_newuserdata(L, sizeof(Mesh**));
  *mesh = &g_mesh;
  luaL_setmetatable(L, "indices");
  return 1;
}

int LuaPrint(lua_State* L)
{
  int nargs = lua_gettop(L);

  for (int i=1; i <= nargs; i++)
  {
    if (lua_isstring(L, i))
    {
      const char* str = lua_tostring(L, i);
      APP.AddMessage(MessageType::Info, str);
    }
    else {

    }
  }

  return 0;
}

int __cdecl luaopen_meshlib(lua_State* L)
{
  string filename("scripts/meshlib.lua");
  if (luaL_loadfile(L, filename.c_str()))
  {
    APP.AddMessage(MessageType::Error, "Error loading " + filename);
    return false;
  }
  else
  {
    APP.AddMessage(MessageType::Info, "Loaded " + filename);
  }

  lua_pcall(L, 0, 0, 0);

  return 1;
}

int __cdecl luaopen_mesh(lua_State* L)
{
  static const struct luaL_Reg vertexFunctions[] = {
    { "__index", VertexIndex },
    { "__newindex", VertexNewIndex },
    NULL, NULL
  };

  static const struct luaL_Reg faceFunctions[] = {
    { "__index", FaceIndex },
    { "__newindex", FaceNewIndex },
    NULL, NULL
  };

  static const struct luaL_Reg printlib[] ={
    { "print", LuaPrint },
    { NULL, NULL } /* end of array */
  };

  // create a metatable for our mesh type
  luaL_newmetatable(L, "vertices");
  luaL_setfuncs(L, vertexFunctions, 0);

  luaL_newmetatable(L, "indices");
  luaL_setfuncs(L, faceFunctions, 0);

  // make our test routine available to Lua
  lua_register(L, "get_vertices", GetMeshVertices);
  lua_register(L, "get_indices", GetMeshIndices);

  // hook up our own print function
  lua_getglobal(L, "_G");
  luaL_setfuncs(L, printlib, 0);
  lua_pop(L, 1);

  return 1;
}


//------------------------------------------------------------------------------
PostProcess::PostProcess(DeferredContext* ctx)
    : _ctx(ctx)
{
}

//------------------------------------------------------------------------------
bool PostProcess::Init()
{
  _cb.Create();

  CD3D11_DEPTH_STENCIL_DESC dsDesc = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT());
  dsDesc.DepthEnable = FALSE;
  dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
  dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
  _depthStencilState = GRAPHICS.CreateDepthStencilState(dsDesc);

  _blendState = GRAPHICS.CreateBlendState(CD3D11_BLEND_DESC(CD3D11_DEFAULT()));

  CD3D11_RASTERIZER_DESC rsDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT());
  rsDesc.CullMode = D3D11_CULL_NONE;
  _rasterizerState = GRAPHICS.CreateRasterizerState(rsDesc);

  CD3D11_SAMPLER_DESC samplerDesc = CD3D11_SAMPLER_DESC(CD3D11_DEFAULT());
  _linearSamplerState = GRAPHICS.CreateSamplerState(samplerDesc);
  
  samplerDesc.AddressU = samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
  _linearWrapSamplerState = GRAPHICS.CreateSamplerState(samplerDesc);

  samplerDesc.AddressU = samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
  _linearBorderSamplerState = GRAPHICS.CreateSamplerState(samplerDesc);

  samplerDesc.AddressU = samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
  samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
  _pointSamplerState = GRAPHICS.CreateSamplerState(samplerDesc);

  GRAPHICS.LoadShadersFromFile("shaders/quad", &_vsQuad, nullptr, nullptr, 0);

  return true;
}

//------------------------------------------------------------------------------
void PostProcess::Setup()
{
  float blendFactor[4] = { 1, 1, 1, 1 };
  _ctx->SetRasterizerState(_rasterizerState);
  _ctx->SetBlendState(_blendState, blendFactor, 0xffffffff);
  _ctx->SetDepthStencilState(_depthStencilState, 0);

  GraphicsObjectHandle samplers[] = { _pointSamplerState, _linearSamplerState,
      _linearWrapSamplerState, _linearBorderSamplerState };
  _ctx->SetSamplers(samplers, 0, 4, ShaderType::PixelShader);

  _ctx->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  _ctx->SetVS(_vsQuad);
  _ctx->SetGS(GraphicsObjectHandle());
}

//------------------------------------------------------------------------------
void PostProcess::Execute(
    const vector<GraphicsObjectHandle>& input,
    GraphicsObjectHandle output,
    GraphicsObjectHandle shader,
    const Color* clearColor,
    WCHAR* name)
{
  D3DPERF_BeginEvent(0xffffffff, name);

  _ctx->SetLayout(GraphicsObjectHandle());

  if (output.IsValid())
    _ctx->SetRenderTarget(output, clearColor);

  _ctx->SetShaderResources(input, ShaderType::PixelShader);

  // set constant buffers
  u32 inputX[8], inputY[8];
  u32 outputX, outputY;

  for (size_t i = 0; i < input.size(); ++i)
    GRAPHICS.GetTextureSize(input[i], &inputX[i], &inputY[i]);
  GRAPHICS.GetTextureSize(output, &outputX, &outputY);
  _cb.data.inputSize.x = (float)inputX[0];
  _cb.data.inputSize.y = (float)inputY[0];
  _cb.data.outputSize.x = (float)outputX;
  _cb.data.outputSize.y = (float)outputY;
  _ctx->SetCBuffer(_cb, ShaderType::PixelShader, 0);

  CD3D11_VIEWPORT viewport = CD3D11_VIEWPORT(0.f, 0.f, (float)outputX, (float)outputY);
  _ctx->SetViewports(viewport, 1);

  _ctx->SetPS(shader);
  _ctx->DrawIndexed(6, 0, 0);

  if (output.IsValid())
    _ctx->UnsetRenderTargets(0, 1);

  _ctx->UnsetSRVs(0, input.size(), ShaderType::PixelShader);

  D3DPERF_EndEvent();
}

//------------------------------------------------------------------------------
GeneratorTest::GeneratorTest(const string &name)
    : Effect(name)
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

  if (_planeConfig.has_camera_pos()) _cameraPos = FromProtocol(_planeConfig.camera_pos());
  if (_planeConfig.has_camera_dir()) _cameraDir = FromProtocol(_planeConfig.camera_dir());
  if (_planeConfig.has_obj_t()) g_mesh.translation = FromProtocol(_planeConfig.obj_t());
  if (_planeConfig.has_obj_r()) g_mesh.rotation = FromProtocol(_planeConfig.obj_r());
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

  _cbBlur.Create();
  _cbBloom.Create();
  _cbComposite.Create();

  _depthStencilState = GRAPHICS.CreateDepthStencilState(CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT()));
  _blendState = GRAPHICS.CreateBlendState(CD3D11_BLEND_DESC(CD3D11_DEFAULT()));
  _rasterizerState = GRAPHICS.CreateRasterizerState(CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT()));

  return true;
}

//------------------------------------------------------------------------------
void GeneratorTest::InitGeneratorScript()
{
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
      u32 vbSize = g_mesh.verts.size() * sizeof(g_mesh.verts[0]);
      if (vbSize > _meshObjects._vbSize)
      {
        while (_meshObjects._vbSize < vbSize)
          _meshObjects._vbSize <<= 1;
        _meshObjects._vb = GRAPHICS.CreateBuffer(D3D11_BIND_VERTEX_BUFFER, _meshObjects._vbSize, true, nullptr, sizeof(PosNormal));
      }

      u32 ibSize = g_mesh.indices.size() * sizeof(u32);
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

      _numIndices = g_mesh.indices.size();
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
      u32 vbSize = g_mesh.verts.size() * sizeof(g_mesh.verts[0]);
      if (vbSize > _meshObjects._vbSize)
      {
        while (_meshObjects._vbSize < vbSize)
          _meshObjects._vbSize <<= 1;
        _meshObjects._vb = GRAPHICS.CreateBuffer(D3D11_BIND_VERTEX_BUFFER, _meshObjects._vbSize, true, nullptr, sizeof(PosNormal));
      }

      u32 ibSize = g_mesh.indices.size() * sizeof(u32);
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

      _numIndices = g_mesh.indices.size();
    }
    lua_pop(_lua, 1);

    _dirtyFlag = false;
  }
}

//------------------------------------------------------------------------------
bool GeneratorTest::Render()
{
  RenderPlane();

  Color black(0, 0, 0, 0);
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

    _ctx->SetRasterizerState(_renderFlags.IsSet(RenderFlags::Wireframe) ? g_mesh._wireframe : g_mesh._solid);
    _ctx->SetCBuffer(_cb, ShaderType::VertexShader, 0);
    _ctx->SetRenderObjects(_meshObjects);
    _ctx->DrawIndexed(_numIndices, 0, 0);

    ScopedRenderTarget rtLuminance(1024, 1024, DXGI_FORMAT_R32_FLOAT,
        BufferFlags(BufferFlag::CreateSrv) | BufferFlag::CreateMipMaps);

    // we want the geometric mean of the luminance, which can be calculated as
    // exp(avg(log(x)), where the avg(log) term can be obtained as the lowest mip level
    // from a texture containing the log luminance
    _postProcess->Setup();
    _postProcess->Execute({_renderTarget}, rtLuminance.h, _psLuminance, 0, L"Luminance");
    _ctx->GenerateMips(rtLuminance.h);

    // luminance level adaption
    _cbToneMapping.data.tau = _planeConfig.tau();
    _cbToneMapping.data.key = _planeConfig.key();
    _cbToneMapping.data.delta = _updateState.delta.total_microseconds() / 1e6f;
    _ctx->SetCBuffer(_cbToneMapping, ShaderType::PixelShader, 1);

    _postProcess->Execute({_luminanceAdaption[!_curAdaption], rtLuminance.h },
        _luminanceAdaption[_curAdaption], _psAdaption, 0, L"Adaption");

    int w, h;
    GRAPHICS.GetBackBufferSize(&w, &h);

    // bloom threshold
    ScopedRenderTarget threshold(w, h, DXGI_FORMAT_R16G16B16A16_FLOAT, BufferFlags(BufferFlag::CreateSrv));
    _cbBloom.data.threshold = _planeConfig.bloom_threshold();
    _ctx->SetCBuffer(_cbBloom, ShaderType::PixelShader, 2);
    _postProcess->Execute({ _renderTarget }, threshold.h, _psThreshold, 0, L"BloomThreshold");

    // blur the bloom
    auto f = BufferFlags(BufferFlag::CreateSrv) | BufferFlag::CreateUav;
    ScopedRenderTarget blur0(w, h, DXGI_FORMAT_R16G16B16A16_FLOAT, f);
    ScopedRenderTarget blur1(w, h, DXGI_FORMAT_R16G16B16A16_FLOAT, f);

    // set constant buffers
    _cbBlur.data.inputSize.x = (float)w;
    _cbBlur.data.inputSize.y = (float)h;
    _cbBlur.data.radius = _planeConfig.blur_radius();
    _ctx->SetCBuffer(_cbBlur, ShaderType::ComputeShader, 0);

    GraphicsObjectHandle srcDst[] =
    {
      threshold.h, blur0.h, blur0.h, blur1.h,
      blur1.h, blur0.h, blur0.h, blur1.h,
      blur1.h, blur0.h, blur0.h, blur1.h,
    };

    for (int i = 0; i < 3; ++i)
    {
      D3DPERF_BeginEvent(0xffffffff, L"blurX");

      _ctx->SetShaderResources({ srcDst[i*4+0] }, ShaderType::ComputeShader);
      _ctx->SetUav(srcDst[i*4+1]);

      _ctx->SetCS(_csBlurX);
      _ctx->Dispatch(h/32+1, 1, 1);

      _ctx->UnsetUAVs(0, 1);
      _ctx->UnsetSRVs(0, 1, ShaderType::ComputeShader);

      D3DPERF_EndEvent();
      D3DPERF_BeginEvent(0xffffffff, L"blurX");

      _ctx->SetShaderResources({ srcDst[i*4+2] }, ShaderType::ComputeShader);
      _ctx->SetUav(srcDst[i*4+3]);

      _ctx->SetCS(_csBlurY);
      _ctx->Dispatch(w/32+1, 1, 1);

      _ctx->UnsetUAVs(0, 1);
      _ctx->UnsetSRVs(0, 1, ShaderType::ComputeShader);

      D3DPERF_EndEvent();
    }

    // calc the bloom

    
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
    ToProtocol(_cameraPos, _planeConfig.mutable_camera_pos());
    ToProtocol(_cameraDir, _planeConfig.mutable_camera_dir());

    ToProtocol(g_mesh.translation, _planeConfig.mutable_obj_t());
    ToProtocol(g_mesh.rotation, _planeConfig.mutable_obj_r());

    fprintf(f, "%s", _planeConfig.DebugString().c_str());
    fclose(f);
  }
  return true;
}

//------------------------------------------------------------------------------
Effect* GeneratorTest::Create(const char* name)
{
  return new GeneratorTest(name);
}

//------------------------------------------------------------------------------
const char* GeneratorTest::Name()
{
  return "generator_test";
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
