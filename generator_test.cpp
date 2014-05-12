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
  vector<float> verts;
  vector<u32> indices;
  vector<Face> faces;

  Vector3 center;
  float radius;

  void Reset()
  {
    indices.clear();
    verts.clear();
    faces.clear();
  }

  void CalcBoundingSphere()
  {
    if (verts.empty())
      return;

    // find center
    Vector3 tmp(verts[0], verts[1], verts[2]);
    for (size_t i = 1*6; i < verts.size() / 6; ++i)
    {
      tmp.x += verts[i * 6 + 0];
      tmp.y += verts[i * 6 + 1];
      tmp.z += verts[i * 6 + 2];
    }

    tmp /= (float)(verts.size() / 6);

    // find max radius to center
    float r = Vector3::DistanceSquared(tmp, Vector3(verts[0], verts[1], verts[2]));
    for (size_t i = 1*6; i < verts.size() / 6; ++i)
    {
      float cand = Vector3::DistanceSquared(tmp, Vector3(verts[i * 6 + 0], verts[i * 6 + 1], verts[i * 6 + 2]));
      r = max(r, cand);
    }

    center = tmp;
    radius = sqrt(r);
  }
};

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

  // create a metatable for our mesh type
  luaL_newmetatable(L, "vertices");
  luaL_setfuncs(L, vertexFunctions, 0);

  luaL_newmetatable(L, "indices");
  luaL_setfuncs(L, faceFunctions, 0);

  // make our test routine available to Lua
  lua_register(L, "get_vertices", GetMeshVertices);
  lua_register(L, "get_indices", GetMeshIndices);
  return 0;
}

class DebugDrawer
{
public:

  static bool Create();
  static bool Destroy();
  static DebugDrawer* Instance();

  void SetContext(DeferredContext* ctx);
  void SetViewProjMatrix(const Matrix& view, const Matrix& proj);
  void SetWorldMatrix(const Matrix& world);
  void SetColor(const Color& color);
  void SetWidth(float width);
  void Reset();

  void DrawMatrix(const Matrix& matrix);
  void DrawSphere(const Vector3& center, float radius);
  void DrawLine(const Vector3& a, const Vector3& b);
  void DrawLineStrip(const Vector3* start, u32 count);

private:

  PosCol* AddQuad(const Vector3& a, const Vector3& b, PosCol* dst);

  bool Init();

  DebugDrawer();
  DISALLOW_COPY_AND_ASSIGN(DebugDrawer);

  static DebugDrawer* _instance;

  GpuObjects _gpuObjects;

  GraphicsObjectHandle _rasterizerState;
  DeferredContext* _ctx;
  Matrix _world;
  Matrix _view;
  Matrix _invView;
  Matrix _proj;
  Color _color;
  float _width;
};

DebugDrawer* DebugDrawer::_instance;

DebugDrawer::DebugDrawer()
{
  Reset();
}

bool DebugDrawer::Create()
{
  if (_instance)
  {
    LOG_WARN("DebugDrawer::Init() called multiple times");
    return true;
  }

  _instance = new DebugDrawer();
  return _instance->Init();
}

bool DebugDrawer::Init()
{
  _gpuObjects.CreateDynamic(64 * 1024, DXGI_FORMAT_R32_UINT, 64 * 1024, sizeof(PosCol), sizeof(CBufferPerFrame));

  if (!LoadShadersFromFile("shaders/debug_draw",
    &_gpuObjects._vs, &_gpuObjects._ps, &_gpuObjects._layout, VF_POS | VF_COLOR))
  {
    LOG_WARN("Error loading shaders for DebugDrawer");
    return false;
  }

  CD3D11_RASTERIZER_DESC desc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT());
  desc.CullMode = D3D11_CULL_NONE;
  _rasterizerState = GRAPHICS.CreateRasterizerState(desc);

  return true;
}

bool DebugDrawer::Destroy()
{
  if (!_instance)
  {
    LOG_WARN("DebugDrawer::Close() called without call to Init()");
    return true;
  }

  return true;
}

DebugDrawer* DebugDrawer::Instance()
{
  return _instance;
}

void DebugDrawer::SetContext(DeferredContext* ctx)
{
  _ctx = ctx;
}

void DebugDrawer::SetViewProjMatrix(const Matrix& view, const Matrix& proj)
{
  _view = view;
  _invView = _view.Invert();
  _proj = proj;
}

void DebugDrawer::SetWorldMatrix(const Matrix& world)
{
  _world = world;
}

void DebugDrawer::SetColor(const Color& color)
{
  _color = color;
}

void DebugDrawer::SetWidth(float width)
{
  _width = width;
}

void DebugDrawer::Reset()
{
  _world = Matrix::Identity();
  _view = Matrix::Identity();
  _proj = Matrix::Identity();
  _color = Color(1, 1, 1, 1);
  _width = 1;
}

PosCol* DebugDrawer::AddQuad(const Vector3& a, const Vector3& b, PosCol* dst)
{
  Vector3 dir(b - a);
  dir.Normalize();
  Vector3 up(_invView.m[1][0], _invView.m[1][1], _invView.m[1][2]);
  Vector3 right = up.Cross(dir);

  // 2 3
  // 0 1
  Vector3 v0 = a - 0.5f * _width * right;
  Vector3 v1 = a + 0.5f * _width * right;
  Vector3 v2 = b - 0.5f * _width * right;
  Vector3 v3 = b + 0.5f * _width * right;

  for (u32 i = 0; i < 6; ++i)
    dst[i].col = _color;

  // 0, 2, 3
  dst[0].pos = v0;
  dst[1].pos = v2;
  dst[2].pos = v3;

  // 0, 3, 1
  dst[3].pos = v0;
  dst[4].pos = v3;
  dst[5].pos = v1;

  return dst + 6;
}


void DebugDrawer::DrawMatrix(const Matrix& matrix)
{

}

void DebugDrawer::DrawSphere(const Vector3& center, float radius)
{
  D3D11_MAPPED_SUBRESOURCE res;
  u32 numVerts = 0;
  if (_ctx->Map(_gpuObjects._vb, 0, D3D11_MAP_WRITE_DISCARD, 0, &res))
  {
    PosCol* vtx = (PosCol*)res.pData;
    PosCol* org = vtx;

    u32 numSteps = 100;
    float inc = 2 * DirectX::XM_PI / numSteps;
    float angle = 0;
    Vector3 a = center + Vector3(0, radius * sin(angle), -radius * cos(angle));
    for (u32 i = 0; i < numSteps; ++i)
    {
      angle += inc;
      Vector3 b = center + Vector3(0, radius * sin(angle), -radius * cos(angle));
      vtx = AddQuad(a, b, vtx);
      a = b;
    }

    angle = 0;
    a = center + Vector3(radius * cos(angle), 0, -radius * sin(angle));
    for (u32 i = 0; i < numSteps; ++i)
    {
      angle += inc;
      Vector3 b = center + Vector3(radius * cos(angle), 0, -radius * sin(angle));
      vtx = AddQuad(a, b, vtx);
      a = b;
    }


    _ctx->Unmap(_gpuObjects._vb, 0);

    numVerts = vtx - org;
  }

  if (numVerts > 0)
  {
    CBufferPerFrame cb;
    cb.world = _world;
    cb.viewProj = (_view * _proj).Transpose();

    _ctx->SetRS(_rasterizerState);
    _ctx->SetCBuffer(_gpuObjects._cbuffer, &cb, sizeof(cb), ShaderType::VertexShader);
    _ctx->SetRenderObjects(_gpuObjects);
    _ctx->Draw(numVerts, 0);
  }

}

void DebugDrawer::DrawLine(const Vector3& a, const Vector3& b)
{

}

void DebugDrawer::DrawLineStrip(const Vector3* start, u32 count)
{

}

//------------------------------------------------------------------------------
GeneratorTest::GeneratorTest(const string &name)
    : Effect(name)
    , _rotatingObject(false)
    , _oldWorldMatrix(Matrix::Identity())
    , _dirtyFlag(true)
    , _lua(nullptr)
    , _numIndices(0)
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

  if (!LoadProto(config, &_config))
    return false;

  _cameraPos = FromProtocol(_config.camera_pos());
  _cameraTarget = FromProtocol(_config.camera_target());
  _oldWorldMatrix = FromProtocol(_config.obj_world());
  _world = _oldWorldMatrix;

  BindConfig(&_config, &_dirtyFlag);

  _meshObjects.CreateDynamic(64 * 1024, DXGI_FORMAT_R32_UINT, 64 * 1024, sizeof(PosNormal), sizeof(CBufferPerFrame));

  LoadShadersFromFile("shaders/generator",
      &_meshObjects._vs, &_meshObjects._ps, &_meshObjects._layout, VF_POS | VF_NORMAL);

  RESOURCE_MANAGER.AddFileWatch("scripts/generator1.lua", 0, true, nullptr, 
    [this](const string& filename, void* token)
    {
      if (_lua)
        lua_close(_lua);

      _lua = luaL_newstate();
      luaL_openlibs(_lua);
      luaL_requiref(_lua, "mesh", luaopen_mesh, 1);

      if (luaL_loadfile(_lua, filename.c_str()))
        return false;

      lua_pcall(_lua, 0, 0, 0);
      _dirtyFlag = true;
      return true;
    });

  if (!DebugDrawer::Create())
    return false;

  return true;
}

//------------------------------------------------------------------------------
bool GeneratorTest::Update(const UpdateState& state)
{
  return true;
}

//------------------------------------------------------------------------------
bool GeneratorTest::Render()
{
  if (_dirtyFlag)
  {
    g_mesh.Reset();

    lua_getglobal(_lua, "generate");
    lua_pushnumber(_lua, _config.radius());
    lua_pushnumber(_lua, _config.height());
    lua_pushinteger(_lua, _config.radial_segments());
    lua_pushinteger(_lua, _config.height_segments());

    int res = lua_pcall(_lua, 4, 0, 0);

    if (res == LUA_OK)
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
      g_mesh.CalcBoundingSphere();
    }

    _dirtyFlag = false;
  }

  _ctx->SetSwapChain(GRAPHICS.DefaultSwapChain(), true);

  if (_numIndices >= 3)
  {
    _ctx->BeginFrame();

    CBufferPerFrame cb;
    _proj = Matrix::CreatePerspectiveFieldOfView(45.0f / 180 * 3.1415f, 16.0f / 10, 1, 1000);
    _view = Matrix::CreateLookAt(
        _cameraPos,
        _cameraTarget,
        Vector3(0, 1, 0));

    cb.world = _world;
    cb.viewProj = (_view * _proj).Transpose();

    _ctx->SetCBuffer(_meshObjects._cbuffer, &cb, sizeof(cb), ShaderType::VertexShader);
    _ctx->SetRenderObjects(_meshObjects);
    _ctx->DrawIndexed(_numIndices, 0, 0);

    DebugDrawer::Instance()->SetContext(_ctx);
    DebugDrawer::Instance()->SetViewProjMatrix(_view, _proj);
    DebugDrawer::Instance()->SetWorldMatrix(_world);
    DebugDrawer::Instance()->SetWidth(5);
    DebugDrawer::Instance()->DrawSphere(g_mesh.center, g_mesh.radius);

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
    ToProtocol(_cameraPos, _config.mutable_camera_pos());
    ToProtocol(_cameraTarget, _config.mutable_camera_target());
    ToProtocol(_oldWorldMatrix, _config.mutable_obj_world());

    fprintf(f, "%s", _config.DebugString().c_str());
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
  DebugDrawer::Instance()->SetColor(Color(_rotatingObject * 1.0f, 1, 0, 1));

  switch (message)
  {
    case WM_LBUTTONDOWN:
    {
      // start of rotation
      u32 xs = LOWORD(lParam);
      u32 ys = HIWORD(lParam);

      Vector3 oView = Vector3(0,0,0);
      Vector3 dView = ScreenToViewSpace(_proj, xs, ys);

      Matrix viewToWorld = _view.Invert();
      Vector3 o = Vector4::Transform(Vector4(oView.x, oView.y, oView.z, 1), viewToWorld);
      Vector3 d = Vector4::Transform(Vector4(dView.x, dView.y, dView.z, 0), viewToWorld);
      d.Normalize();

      Vector3 center_ws = Vector4::Transform(Vector4(g_mesh.center.x, g_mesh.center.y, g_mesh.center.z, 1), _world);
      float t = Raycast(center_ws, g_mesh.radius, o, d);
      _rotatingObject = t > 0;
      if (_rotatingObject)
        _v0 = d;

      break;
    }

    case WM_LBUTTONUP:
      _oldWorldMatrix = _world;
      _rotatingObject = false;
      break;

    case WM_MOUSEMOVE:
    {
      if (_rotatingObject)
      {
        u32 xs = LOWORD(lParam);
        u32 ys = HIWORD(lParam);

        Matrix viewToWorld = _view.Invert();

        // Find the click point in world space
        Vector3 pos = ScreenToViewSpace(_proj, xs, ys);

        // Vector from eye pos (0,0,0) to click point in world space
        Vector3 v1 = Vector4::Transform(Vector4(pos.x, pos.y, pos.z, 0), viewToWorld);
        v1.Normalize();

        // angle between the two vectors
        float speed = 2;
        float angle = acos(_v0.Dot(v1));
        Vector3 axis = _v0.Cross(v1);
        if (axis.Length() > 0)
          _world = Matrix::CreateFromAxisAngle(axis, speed * angle) * _oldWorldMatrix;
      }
      break;
    }

    case WM_MOUSEWHEEL:
    {
      s16 delta = HIWORD(wParam);
      float fDelta = delta / 120.0f;
      Vector3 dir = _cameraTarget - _cameraPos;
      dir.Normalize();
      _cameraPos += 10 * fDelta * dir;
      break;
    }
    
  }
}
