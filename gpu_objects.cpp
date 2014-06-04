#include "gpu_objects.hpp"
#include "graphics.hpp"

using namespace boba;

void GpuObjects::CreateDynamic(
  u32 ibSize, DXGI_FORMAT ibFormat,
  u32 vbSize, u32 vbElemSize)
{
  _ibSize = ibSize;
  _vbSize = vbSize;
  _ib = GRAPHICS.CreateBuffer(D3D11_BIND_INDEX_BUFFER, ibSize, true, nullptr, ibFormat);
  _vb = GRAPHICS.CreateBuffer(D3D11_BIND_VERTEX_BUFFER, vbSize, true, nullptr, vbElemSize);
}
