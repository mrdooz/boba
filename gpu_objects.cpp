#include "gpu_objects.hpp"
#include "graphics.hpp"
#include "deferred_context.hpp"

using namespace boba;
using namespace bristol;

//------------------------------------------------------------------------------
bool GpuObjects::CreateDynamic(
  u32 ibSize, DXGI_FORMAT ibFormat,
  u32 vbSize, u32 vbElemSize)
{
  return CreateDynamicIb(ibSize, ibFormat, nullptr) && CreateDynamicVb(vbSize, vbElemSize, nullptr);
}

//------------------------------------------------------------------------------
bool GpuObjects::CreateDynamic(
    u32 ibSize, DXGI_FORMAT ibFormat, const void* ibData,
    u32 vbSize, u32 vbElemSize, const void* vbData)
{
  return CreateDynamicIb(ibSize, ibFormat, ibData) && CreateDynamicVb(vbSize, vbElemSize, vbData);
}

//------------------------------------------------------------------------------
bool GpuObjects::CreateDynamicVb(u32 vbSize, u32 vbElemSize, const void* vbData)
{
  _vbSize = vbSize;
  _vbElemSize = vbElemSize;
  _vb = GRAPHICS.CreateBuffer(D3D11_BIND_VERTEX_BUFFER, vbSize, true, vbData, vbElemSize);
  return _vb.IsValid();
}

//------------------------------------------------------------------------------
bool GpuObjects::CreateDynamicIb(u32 ibSize, DXGI_FORMAT ibFormat, const void* ibData)
{
  _ibSize = ibSize;
  _ibFormat = ibFormat;
  _ib = GRAPHICS.CreateBuffer(D3D11_BIND_INDEX_BUFFER, ibSize, true, ibData, ibFormat);
  return _ib.IsValid();
}

//------------------------------------------------------------------------------
bool GpuObjects::LoadShadersFromFile(
    const char* filename,
    const char* vsEntry,
    const char* psEntry,
    u32 flags)
{
  return GRAPHICS.LoadShadersFromFile(filename, 
      vsEntry ? &_vs : nullptr, 
      psEntry ? &_ps : nullptr, 
      vsEntry ? &_layout : nullptr, 
      VF_POS | VF_NORMAL,
      vsEntry,
      psEntry);
}

//------------------------------------------------------------------------------
bool GpuState::Create(
  const D3D11_DEPTH_STENCIL_DESC& dssDesc,
  const D3D11_BLEND_DESC& blendDesc,
  const D3D11_RASTERIZER_DESC& rasterizerDesc)
{
  _depthStencilState = GRAPHICS.CreateDepthStencilState(dssDesc);
  _blendState = GRAPHICS.CreateBlendState(blendDesc);
  _rasterizerState = GRAPHICS.CreateRasterizerState(rasterizerDesc);

  CD3D11_SAMPLER_DESC samplerDesc = CD3D11_SAMPLER_DESC(CD3D11_DEFAULT());
  _samplers[Linear] = GRAPHICS.CreateSamplerState(samplerDesc);

  samplerDesc.AddressU = samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
  _samplers[LinearWrap] = GRAPHICS.CreateSamplerState(samplerDesc);

  samplerDesc.AddressU = samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
  _samplers[LinearBorder] = GRAPHICS.CreateSamplerState(samplerDesc);

  samplerDesc.AddressU = samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
  samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
  _samplers[Point] = GRAPHICS.CreateSamplerState(samplerDesc);

  return
    _depthStencilState.IsValid() && _blendState.IsValid() && _rasterizerState.IsValid() &&
    _samplers[0].IsValid() && _samplers[1].IsValid() && _samplers[2].IsValid() && _samplers[3].IsValid();
}

//------------------------------------------------------------------------------
bool GpuState::Create()
{
  return Create(
    CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT()),
    CD3D11_BLEND_DESC(CD3D11_DEFAULT()),
    CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT()));
}
