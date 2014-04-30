#include "generator_test.hpp"
#include "graphics.hpp"
#include "deferred_context.hpp"
#include "app.hpp"
#include "demo_engine.hpp"
#include "proto_helpers.hpp"
#include "resource_manager.hpp"
#include "scene.hpp"
#include "graphics_utils.hpp"
#include "vertex_types.hpp"

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

namespace geometry
{
  struct Face
  {
    Face() {}
    Face(u32 a, u32 b, u32 c) : a(a), b(b), c(c) {}
    u32 a, b, c;
    Vector3 normal;
  };

  struct Mesh
  {
    vector<PosNormal> verts;
    vector<u32> indices;
    vector<Face> faces;
  };

  void CreateCylinder(
      float width,
      float height,
      float length,
      u32 heightSegments,
      u32 radialSegments,
      Mesh* mesh)
  {
    float y = 0;
    for (u32 i = 0; i < heightSegments+1; ++i)
    {
      float inc = 2 * DirectX::XM_PI / radialSegments;
      float angle = 0;
      for (u32 j = 0; j < radialSegments; ++j)
      {
        // create a ring
        float x = width * cos(angle);
        float z = width * sin(angle);
        angle += inc;

        mesh->verts.push_back(PosNormal(x, y, z, cos(angle), 0, sin(angle)));

        if (i < heightSegments)
        {
          // c-d
          // |\|
          // a-b

          u32 a = (i + 0) * radialSegments + j;
          u32 b = (i + 0) * radialSegments + (j + 1) % radialSegments;
          u32 c = (i + 1) * radialSegments + j;
          u32 d = (i + 1) * radialSegments + (j + 1) % radialSegments;

          mesh->faces.push_back(Face(a, c, b));
          mesh->faces.push_back(Face(b, c, d));

          mesh->indices.push_back(a);
          mesh->indices.push_back(c);
          mesh->indices.push_back(b);

          mesh->indices.push_back(b);
          mesh->indices.push_back(c);
          mesh->indices.push_back(d);

        }
      }
      y += height / heightSegments;
    }
  }
}


//------------------------------------------------------------------------------
GeneratorTest::GeneratorTest(const string &name)
    : Effect(name)
    , _rotatingObject(false)
    , _oldWorldMatrix(Matrix::Identity())
    , _dirtyFlag(true)
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

  BindConfig(&_config, &_dirtyFlag);

  _vb = GRAPHICS.CreateBuffer(D3D11_BIND_VERTEX_BUFFER, 64 * 1024, true, nullptr, sizeof(PosNormal));
  _ib = GRAPHICS.CreateBuffer(D3D11_BIND_INDEX_BUFFER, 64 * 1024, true, nullptr, DXGI_FORMAT_R32_UINT);
  _cbuffer = GRAPHICS.CreateBuffer(D3D11_BIND_CONSTANT_BUFFER, sizeof(CBufferPerFrame), true);

  _world = Matrix::Identity();

  LoadShadersFromFile("shaders/generator", &_vs, &_ps, &_layout, Mesh::VF_POS | Mesh::VF_NORMAL);

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
  _ctx->SetSwapChain(GRAPHICS.DefaultSwapChain(), true);
  _ctx->BeginFrame();

  CBufferPerFrame cb;
  _proj = Matrix::CreatePerspectiveFieldOfView(45.0f/180*3.1415f, 16.0f/10, 1, 1000);
  _view = Matrix::CreateLookAt(
    Vector3(0, 0, -150),
    Vector3(0, 0, 0),
    Vector3(0, 1, 0));

  if (_dirtyFlag)
  {
    // recreate the geometry
    geometry::Mesh mesh;
    geometry::CreateCylinder(
        _config.width(), _config.height(), _config.length(),
        _config.height_segments(), _config.radial_segments(), &mesh);

    D3D11_MAPPED_SUBRESOURCE res;
    if (_ctx->Map(_vb, 0, D3D11_MAP_WRITE_DISCARD, 0, &res))
    {
      memcpy(res.pData, mesh.verts.data(), mesh.verts.size() * sizeof(mesh.verts[0]));
      _ctx->Unmap(_vb, 0);
    }

    if (_ctx->Map(_ib, 0, D3D11_MAP_WRITE_DISCARD, 0, &res))
    {
      memcpy(res.pData, mesh.indices.data(), mesh.indices.size() * sizeof(u32));
      _ctx->Unmap(_ib, 0);
    }

    _numIndices = mesh.indices.size();

    _dirtyFlag = false;
  }

   cb.world = _world;
   cb.viewProj = (_view * _proj).Transpose();
 
   _ctx->SetCBuffer(_cbuffer, &cb, sizeof(cb), ShaderType::VertexShader);
 
   _ctx->SetVS(_vs);
   _ctx->SetPS(_ps);
   _ctx->SetLayout(_layout);
   _ctx->SetVB(_vb);
   _ctx->SetIB(_ib);
   _ctx->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
   _ctx->DrawIndexed(_numIndices, 0, 0);

  _ctx->EndFrame();

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
/*
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
*/
}
