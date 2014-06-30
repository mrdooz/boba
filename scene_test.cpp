#include "scene_test.hpp"
#include "graphics.hpp"
#include "deferred_context.hpp"
#include "app.hpp"
#include "demo_engine.hpp"
#include "proto_helpers.hpp"
#include "resource_manager.hpp"
#include "scene.hpp"

#include "boba_io.hpp"

using namespace boba;
using namespace bristol;

namespace
{
  struct CBufferPerFrame
  {
    Matrix world;
    Matrix viewProj;
  };
}


Mesh mesh;

struct BobaLoader
{
  bool Load(const char* filename)
  {
    if (!LoadFile(filename, &buf))
      return false;

    const BobaScene* scene = (const BobaScene*)&buf[0];

    if (strncmp(scene->id, "boba", 4) != 0)
      return false;

    ProcessFixups(scene->fixupOffset);

    char* ptr;

    // add meshes
    ptr = &buf[scene->elementOffset[(u32)SceneElement::Mesh]];
    u32 numMeshes = *(u32*)ptr;
    MeshElement* element = (MeshElement*)(ptr + 4);
    for (u32 i = 0; i < numMeshes; ++i, ++element)
    {
      meshes.push_back(element);
    }
    return true;
  }

  bool LoadMesh(const string& name, u32 flags, Mesh* mesh)
  {
    for (const MeshElement* e : meshes)
    {
      if (e->name == name)
      {
        mesh->_name = name;
        mesh->_numVerts = e->numVerts;
        mesh->_numIndices = e->numIndices;
        mesh->_boundingSphere = BoundingSphere(XMFLOAT3(e->bx, e->by, e->bz), e->br);

        // create a combined buffer for the vertex data
        bool hasPos = flags & VF_POS && e->verts;
        bool hasNormal = flags & VF_NORMAL && e->normals;
        bool hasUv = flags & VF_UV && e->uv;

        u32 vertexSize = (hasPos ? 3 : 0) + (hasNormal ? 3 : 0) + (hasUv ? 2 : 0);

        vector<u8> buf(vertexSize * e->numVerts * sizeof(float));
        float* ptr = (float*)buf.data();

        // interleave the vertex data..
        u32 numVerts = e->numVerts;
        u32 ofs = 0;
        if (hasPos)
        {
          for (u32 i = 0; i < numVerts; ++i)
            memcpy(&ptr[i*vertexSize], &e->verts[i*3], 3 * sizeof(float));
          ofs += 3;
        }

        if (hasNormal)
        {
          for (u32 i = 0; i < numVerts; ++i)
            memcpy(&ptr[i*vertexSize + ofs], &e->normals[i*3], 3 * sizeof(float));
          ofs += 3;
        }

        if (hasUv)
        {
          for (u32 i = 0; i < numVerts; ++i)
            memcpy(&ptr[i*vertexSize + ofs], &e->uv[i*2], 2 * sizeof(float));
          ofs += 2;
        }

        mesh->_vb = GRAPHICS.CreateBuffer(
            D3D11_BIND_VERTEX_BUFFER,
            e->numVerts * vertexSize * sizeof(float),
            false,
            buf.data(),
            vertexSize * sizeof(float));

        mesh->_ib = GRAPHICS.CreateBuffer(
            D3D11_BIND_INDEX_BUFFER,
            e->numIndices * sizeof(u32),
            false,
            e->indices,
            DXGI_FORMAT_R32_UINT);

        return true;
      }
    }

    return false;
  }

  void ProcessFixups(u32 fixupOffset)
  {
    // Process all the fixups. A list of locations that point to relative
    // data is stored (the fixup list), and for each of these locations, we
    // add the base of the file we loaded, converting the fixups to valid
    // memory locations
    u32* fixups = (u32*)&buf[fixupOffset];
    u32 numFixups = *fixups++;
    u32 base = (u32)&buf[0];
    for (u32 i = 0; i < numFixups; ++i)
    {
      u32 fixup = fixups[i];
      *(u32*)(base + fixup) += base;
    }
  }

  vector<MeshElement*> meshes;
  vector<char> buf;
};

//------------------------------------------------------------------------------
SceneTest::SceneTest(const string &name)
    : Effect(name)
    , _rotatingObject(false)
    , _oldWorldMatrix(Matrix::Identity())
{
}

//------------------------------------------------------------------------------
SceneTest::~SceneTest()
{
}

//------------------------------------------------------------------------------
bool SceneTest::Init(const char* config)
{
  BobaLoader loader;
  loader.Load("D:\\projects\\model\\thing1.lxo.boba");
  u32 vertexFlags = VF_POS | VF_NORMAL;
  if (!loader.LoadMesh("mesh1", vertexFlags, &mesh))
    return false;

  if (!GRAPHICS.LoadShadersFromFile("shaders/simple_mesh", &_vs, &_ps, &mesh._layout, vertexFlags))
    return false;

  _configName = config;

  if (!LoadProto(config, &_config))
    return false;

//  BindConfig(&_config);

  CD3D11_SAMPLER_DESC sampler = CD3D11_SAMPLER_DESC(CD3D11_DEFAULT());
  _samplerState = GRAPHICS.CreateSamplerState(sampler);
  if (!_samplerState.IsValid())
    return false;

  _cbuffer = GRAPHICS.CreateBuffer(D3D11_BIND_CONSTANT_BUFFER, sizeof(CBufferPerFrame), true);
  if (!_cbuffer.IsValid())
    return false;

  return true;
}

//------------------------------------------------------------------------------
bool SceneTest::Update(const UpdateState& state)
{
  return true;
}

//------------------------------------------------------------------------------
bool SceneTest::Render()
{
  _ctx->SetSwapChain(GRAPHICS.DefaultSwapChain(), true);
  _ctx->BeginFrame();

  CBufferPerFrame cb;
  _proj = Matrix::CreatePerspectiveFieldOfView(45.0f/180*3.1415f, 16.0f/10, 1, 1000);
  _view = Matrix::CreateLookAt(
    Vector3(0, 0, -50),
    Vector3(0, 0, 0),
    Vector3(0, 1, 0));

  cb.world = mesh._world;
  cb.viewProj = (_view * _proj).Transpose();

  _ctx->SetCBuffer(_cbuffer, &cb, sizeof(cb), ShaderType::VertexShader, 0);

  _ctx->SetVS(_vs);
  _ctx->SetPS(_ps);
  _ctx->SetLayout(mesh._layout);
  _ctx->SetVB(mesh._vb);
  _ctx->SetIB(mesh._ib);
  _ctx->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  _ctx->DrawIndexed(mesh._numIndices, 0, 0);

  _ctx->EndFrame();

  return true;
}

//------------------------------------------------------------------------------
bool SceneTest::Close()
{
  return true;
}

//------------------------------------------------------------------------------
bool SceneTest::SaveSettings()
{
  if (FILE* f = fopen(_configName.c_str() ,"wt"))
  {
    fprintf(f, "%s", _config.DebugString().c_str());
    fclose(f);
  }
  return true;
}

//------------------------------------------------------------------------------
Effect* SceneTest::Create(const char* name)
{
  return new SceneTest(name);
}

//------------------------------------------------------------------------------
const char* SceneTest::Name()
{
  return "scene_test";
}

//------------------------------------------------------------------------------
float SceneTest::Raycast(const Vector3& o, const Vector3& d)
{
  // ray is in view space
  XMFLOAT3 xx = mesh._boundingSphere.Center;
  Vector3 ro(xx.x, xx.y, xx.z);
  float r = mesh._boundingSphere.Radius;

  // ray -> sphere intersection
  // Compute A, B and C coefficients
  float a = Dot(d, d);
  float b = 2 * Dot(d, o - ro);
  float c = Dot(o - ro, o - ro) - (r * r);

  // Find discriminant
  float disc = b * b - 4 * a * c;

  // if discriminant is negative there are no real roots, so ray misses sphere
  if (disc < 0)
    return -1;

  float t0 = (-b - sqrt(disc)) / (2 * a);
  float t1 = (-b + sqrt(disc)) / (2 * a);

  // return smallest non-negative 
  float ta = min(t0, t1);
  float tb = max(t0, t1);
  return ta < 0 ? tb : ta;
}

//------------------------------------------------------------------------------
Vector3 SceneTest::ScreenToViewSpace(u32 x, u32 y)
{
  int w, h;
  GRAPHICS.GetBackBufferSize(&w, &h);
  float xv = (2.0f * x / w - 1) / _proj(0, 0);
  float yv = (-2.0f * y / h + 1) / _proj(1, 1);

  return Vector3(xv, yv, 1);
}

//------------------------------------------------------------------------------
void SceneTest::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message)
  {
    case WM_LBUTTONDOWN:
    {
      // start of rotation
      u32 xs = LOWORD(lParam);
      u32 ys = HIWORD(lParam);

      Vector3 oView = Vector3(0,0,0);
      Vector3 dView = ScreenToViewSpace(xs, ys);

      Matrix viewToWorld = _view.Invert();
      Vector3 o = Vector4::Transform(Vector4(oView.x, oView.y, oView.z, 1), viewToWorld);
      Vector3 d = Vector4::Transform(Vector4(dView.x, dView.y, dView.z, 0), viewToWorld);
      d.Normalize();

      float t = Raycast(o, d);
      _rotatingObject = t > 0;
      if (_rotatingObject)
        _v0 = d;
      break;
    }

    case WM_LBUTTONUP:
      _oldWorldMatrix = mesh._world;
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
        Vector3 pos = ScreenToViewSpace(xs, ys);

        // Vector from eye pos (0,0,0) to click point in world space
        Vector3 v1 = Vector4::Transform(Vector4(pos.x, pos.y, pos.z, 0), viewToWorld);
        v1.Normalize();

        // angle between the two vectors
        float speed = 2;
        float angle = acos(_v0.Dot(v1));
        Vector3 axis = _v0.Cross(v1);
        if (axis.Length() > 0)
          mesh._world = Matrix::CreateFromAxisAngle(axis, speed * angle) * _oldWorldMatrix;
      }
      break;
    }
  }
}
