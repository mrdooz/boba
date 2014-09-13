#pragma once
#include "graphics.hpp"
#include "object_handle.hpp"

namespace boba
{
  class DeferredContext;

  // Note, the objects here just hold state. The code for actually setting the state
  // on the context is found in DeferredContext (SetGpuState et al)

  //------------------------------------------------------------------------------
  struct GpuObjects
  {
    bool CreateDynamic(
        u32 ibSize, DXGI_FORMAT ibFormat,
        u32 vbSize, u32 vbElemSize);

    bool CreateDynamic(
        u32 ibSize, DXGI_FORMAT ibFormat, const void* ibData,
        u32 vbSize, u32 vbElemSize, const void* vbData);

    bool CreateDynamicVb(u32 vbSize, u32 vbElemSize, const void* vbData = nullptr);
    bool CreateDynamicIb(u32 ibSize, DXGI_FORMAT ibFormat, const void* ibData = nullptr);

    bool LoadShadersFromFile(const char* filename, const char* vsEntry, const char* psEntry, u32 flags);

    ObjectHandle _vs;
    ObjectHandle _ps;
    ObjectHandle _layout;

    ObjectHandle _vb;
    ObjectHandle _ib;

    u32 _vbSize;
    u32 _ibSize;
    u32 _vbElemSize;
    DXGI_FORMAT _ibFormat;
  };

  //------------------------------------------------------------------------------
  template <typename T>
  struct ConstantBuffer
  {
    bool Create()
    {
      handle = GRAPHICS.CreateBuffer(D3D11_BIND_CONSTANT_BUFFER, sizeof(T), true);
      return handle.IsValid();
    }

    T data;
    ObjectHandle handle;
  };

  //------------------------------------------------------------------------------
  struct GpuState
  {
    bool Create();
    bool Create(
        const D3D11_DEPTH_STENCIL_DESC& dssDesc,
        const D3D11_BLEND_DESC& blendDesc,
        const D3D11_RASTERIZER_DESC& rasterizerDesc);

    ObjectHandle _depthStencilState;
    ObjectHandle _blendState;
    ObjectHandle _rasterizerState;

    enum Samplers 
    {
      Linear,
      LinearWrap,
      LinearBorder,
      Point,
    };

    ObjectHandle _samplers[4];
  };
}
