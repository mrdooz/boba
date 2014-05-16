#include "generator_test.hpp"
#include "graphics.hpp"
#include "deferred_context.hpp"
#include "app.hpp"
#include "demo_engine.hpp"
#include "proto_helpers.hpp"
#include "resource_manager.hpp"
#include "graphics_utils.hpp"
#include "vertex_types.hpp"
#include "error.hpp"

#include "boba_io.hpp"
#include "protocol/generator_bindings.hpp"
#include "debug_drawer.hpp"

using namespace boba;

namespace
{
  struct CBufferPerFrame
  {
    Matrix world;
    Matrix viewProj;
  };
}

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
GeneratorTest::GeneratorTest(const string &name)
    : Effect(name)
    , _rotatingObject(false)
    , _dirtyFlag(true)
    , _lua(nullptr)
    , _numIndices(0)
    , _debugDraw(false)
    , _wireframe(true)
    , _cameraDir(0,0,1)
{
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
  BindPlane(&_planeConfig, &_dirtyFlag);

  _meshObjects.CreateDynamic(64 * 1024, DXGI_FORMAT_R32_UINT, 64 * 1024, sizeof(PosNormal), sizeof(CBufferPerFrame));

  LoadShadersFromFile("shaders/generator",
      &_meshObjects._vs, &_meshObjects._ps, &_meshObjects._layout, VF_POS | VF_NORMAL);

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

  if (!DebugDrawer::Create())
    return false;


  CD3D11_RASTERIZER_DESC desc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT());
  desc.FillMode = D3D11_FILL_SOLID;
  g_mesh._solid = GRAPHICS.CreateRasterizerState(desc);
  desc.FillMode = D3D11_FILL_WIREFRAME;
  desc.CullMode = D3D11_CULL_NONE;
  g_mesh._wireframe = GRAPHICS.CreateRasterizerState(desc);

  return true;
}

//------------------------------------------------------------------------------
bool GeneratorTest::Update(const UpdateState& state)
{
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

  _ctx->SetSwapChain(GRAPHICS.DefaultSwapChain(), true);

  if (_numIndices >= 3)
  {
    _ctx->BeginFrame();

    CBufferPerFrame cb;
    _proj = Matrix::CreatePerspectiveFieldOfView(45.0f / 180 * 3.1415f, 16.0f / 10, 1, 1000);
    _view = Matrix::CreateLookAt(
        _cameraPos,
        _cameraPos + 100 * _cameraDir,
        Vector3(0, 1, 0));
    _invView = _view.Invert();

    Matrix world = g_mesh.World();
    cb.world = world.Transpose();
    cb.viewProj = (_view * _proj).Transpose();

    _ctx->SetRS(_wireframe ? g_mesh._wireframe : g_mesh._solid);

    _ctx->SetCBuffer(_meshObjects._cbuffer, &cb, sizeof(cb), ShaderType::VertexShader);
    _ctx->SetRenderObjects(_meshObjects);
    _ctx->DrawIndexed(_numIndices, 0, 0);

    if (_debugDraw)
    {
      DEBUG_DRAW.SetContext(_ctx);
      DEBUG_DRAW.SetViewProjMatrix(_view, _proj);
      DEBUG_DRAW.SetWorldMatrix(world);
      DEBUG_DRAW.SetWidth(5);
      DEBUG_DRAW.DrawSphere(g_mesh.center, g_mesh.radius);
    }

    _ctx->EndFrame();
  }

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
        _wireframe = !_wireframe;
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