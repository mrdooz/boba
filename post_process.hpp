#pragma once
#include "object_handle.hpp"
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
      const vector<ObjectHandle>& input,
      ObjectHandle output,
      ObjectHandle shader,
      const Color* clearColor,
      WCHAR* name);

    DeferredContext* _ctx;

    struct CBufferPS
    {
      Vector2 inputSize;
      Vector2 outputSize;
    };
    ConstantBuffer<CBufferPS> _cb;

    GpuState _gpuState;

    ObjectHandle _vsQuad;
  };
}