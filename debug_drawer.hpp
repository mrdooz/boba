#pragma once
#include "gpu_objects.hpp"

namespace bristol
{
  struct PosCol;
}

namespace boba
{
  class DeferredContext;

  class DebugDrawer
  {
  public:

    static bool Create();
    static bool Destroy();
    static DebugDrawer& Instance();

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

    struct CBufferPerFrame
    {
      Matrix world;
      Matrix viewProj;
    };

    bristol::PosCol* AddQuad(const Vector3& a, const Vector3& b, bristol::PosCol* dst);

    bool Init();

    DebugDrawer();
    DISALLOW_COPY_AND_ASSIGN(DebugDrawer);

    static DebugDrawer* _instance;

    ConstantBuffer<CBufferPerFrame> _cb;
    GpuObjects _gpuObjects;

    GraphicsObjectHandle _rasterizerState;
    DeferredContext* _ctx;

    // TODO: these should be grouped into state objects, and the draw calls
    // collected together and issued at once
    Matrix _world;
    Matrix _view;
    Matrix _invView;
    Matrix _proj;
    Color _color;
    float _width;
  };

}