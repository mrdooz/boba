#pragma once
#include "graphics_object_handle.hpp"

namespace boba
{
  struct GpuObjects
  {
    void CreateDynamic(
        u32 ibSize, DXGI_FORMAT ibFormat,
        u32 vbSize, u32 vbElemSize,
        u32 cbSize);

    GraphicsObjectHandle _vs;
    GraphicsObjectHandle _ps;
    GraphicsObjectHandle _layout;

    GraphicsObjectHandle _vb;
    GraphicsObjectHandle _ib;
    GraphicsObjectHandle _cbuffer;

    u32 _vbSize;
    u32 _ibSize;
  };
}
