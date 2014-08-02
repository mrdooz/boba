#pragma once
#include "graphics.hpp"
#include "graphics_object_handle.hpp"

namespace boba
{
  struct GpuObjects
  {
    void CreateDynamic(
        u32 ibSize, DXGI_FORMAT ibFormat,
        u32 vbSize, u32 vbElemSize);

    void CreateDynamic(
        u32 ibSize, DXGI_FORMAT ibFormat, const void* ibData,
        u32 vbSize, u32 vbElemSize, const void* vbData);

    GraphicsObjectHandle _vs;
    GraphicsObjectHandle _ps;
    GraphicsObjectHandle _layout;

    GraphicsObjectHandle _vb;
    GraphicsObjectHandle _ib;

    u32 _vbSize;
    u32 _ibSize;
  };

  template <typename T>
  struct ConstantBuffer
  {
    bool Create()
    {
      handle = GRAPHICS.CreateBuffer(D3D11_BIND_CONSTANT_BUFFER, sizeof(T), true);
      return handle.IsValid();
    }

    T data;
    GraphicsObjectHandle handle;
  };

}
