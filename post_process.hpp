#pragma once
#include "graphics_object_handle.hpp"
#include "gpu_objects.hpp"

namespace boba
{
  class DeferredContext;

  struct PostProcess
  {
    PostProcess(DeferredContext* ctx);

    bool Init();
    void Setup();

    void Execute(
      const vector<GraphicsObjectHandle>& input,
      GraphicsObjectHandle output,
      GraphicsObjectHandle shader,
      const Color* clearColor,
      WCHAR* name);

    DeferredContext* _ctx;

    struct CBufferPS
    {
      Vector2 inputSize;
      Vector2 outputSize;
    };
    ConstantBuffer<CBufferPS> _cb;

    GraphicsObjectHandle _depthStencilState;
    GraphicsObjectHandle _blendState;
    GraphicsObjectHandle _rasterizerState;

    GraphicsObjectHandle _linearSamplerState;
    GraphicsObjectHandle _linearWrapSamplerState;
    GraphicsObjectHandle _linearBorderSamplerState;
    GraphicsObjectHandle _pointSamplerState;

    GraphicsObjectHandle _vsQuad;
  };
}