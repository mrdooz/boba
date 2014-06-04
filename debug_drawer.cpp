#include "debug_drawer.hpp"
#include "error.hpp"
#include "vertex_types.hpp"
#include "graphics_utils.hpp"
#include "graphics.hpp"
#include "deferred_context.hpp"

using namespace boba;

//------------------------------------------------------------------------------
DebugDrawer* DebugDrawer::_instance;

//------------------------------------------------------------------------------
DebugDrawer::DebugDrawer()
{
  Reset();
}

//------------------------------------------------------------------------------
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
  _gpuObjects.CreateDynamic(64 * 1024, DXGI_FORMAT_R32_UINT, 64 * 1024, sizeof(PosCol));
  _cb.Create();

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

//------------------------------------------------------------------------------
bool DebugDrawer::Destroy()
{
  if (!_instance)
  {
    LOG_WARN("DebugDrawer::Close() called without call to Init()");
    return true;
  }

  return true;
}

//------------------------------------------------------------------------------
DebugDrawer& DebugDrawer::Instance()
{
  return *_instance;
}

//------------------------------------------------------------------------------
void DebugDrawer::SetContext(DeferredContext* ctx)
{
  _ctx = ctx;
}

//------------------------------------------------------------------------------
void DebugDrawer::SetViewProjMatrix(const Matrix& view, const Matrix& proj)
{
  _view = view;
  _invView = _view.Invert();
  _proj = proj;
}

//------------------------------------------------------------------------------
void DebugDrawer::SetWorldMatrix(const Matrix& world)
{
  _world = world;
}

//------------------------------------------------------------------------------
void DebugDrawer::SetColor(const Color& color)
{
  _color = color;
}

//------------------------------------------------------------------------------
void DebugDrawer::SetWidth(float width)
{
  _width = width;
}

//------------------------------------------------------------------------------
void DebugDrawer::Reset()
{
  _world = Matrix::Identity();
  _view = Matrix::Identity();
  _proj = Matrix::Identity();
  _color = Color(1, 1, 1, 1);
  _width = 1;
}

//------------------------------------------------------------------------------
PosCol* DebugDrawer::AddQuad(const Vector3& a, const Vector3& b, PosCol* dst)
{
  Vector3 dir(b - a);
  dir.Normalize();
  // use the camera's up vector
  Vector3 up(_invView.m[1][0], _invView.m[1][1], _invView.m[1][2]);
  up.Normalize();
  Vector3 right = up.Cross(dir);
  right.Normalize();

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


//------------------------------------------------------------------------------
void DebugDrawer::DrawMatrix(const Matrix& matrix)
{

}

//------------------------------------------------------------------------------
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
    cb.world = _world.Transpose();
    cb.viewProj = (_view * _proj).Transpose();

    _ctx->SetRasterizerState(_rasterizerState);
    _ctx->SetCBuffer(_cb, ShaderType::VertexShader);
    _ctx->SetRenderObjects(_gpuObjects);
    _ctx->Draw(numVerts, 0);
  }

}

//------------------------------------------------------------------------------
void DebugDrawer::DrawLine(const Vector3& a, const Vector3& b)
{

}

//------------------------------------------------------------------------------
void DebugDrawer::DrawLineStrip(const Vector3* start, u32 count)
{

}
