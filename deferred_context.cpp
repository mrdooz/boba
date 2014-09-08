#include "graphics.hpp"
#include "deferred_context.hpp"
#include "gpu_objects.hpp"

static const int MAX_SAMPLERS = 8;
static const int MAX_TEXTURES = 8;

using namespace boba;

//------------------------------------------------------------------------------
DeferredContext::DeferredContext() 
  : _ctx(nullptr)
  , _is_immediate_context(false)
{
    _default_stencil_ref = GRAPHICS.DefaultStencilRef();
    memcpy(_default_blend_factors, GRAPHICS.DefaultBlendFactors(), sizeof(_default_blend_factors));
    _default_sample_mask = GRAPHICS.DefaultSampleMask();
}

//------------------------------------------------------------------------------
void DeferredContext::GenerateMips(GraphicsObjectHandle h)
{
  auto r = GRAPHICS._renderTargets.Get(h)->srv.resource;
  _ctx->GenerateMips(r);
}

//------------------------------------------------------------------------------
void DeferredContext::SetRenderTarget(GraphicsObjectHandle render_target, const Color* clearColor)
{
  SetRenderTargets(&render_target, clearColor, 1);
}

//------------------------------------------------------------------------------
void DeferredContext::SetRenderTargets(
    GraphicsObjectHandle* renderTargets,
    const Color* clearTargets,
    int numRenderTargets)
{
  if (!numRenderTargets)
    return;

  ID3D11RenderTargetView *rts[8] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
  ID3D11DepthStencilView *dsv = nullptr;
  D3D11_TEXTURE2D_DESC texture_desc;

  // Collect the valid render targets, set the first available depth buffer
  // and clear targets if specified
  for (int i = 0; i < numRenderTargets; ++i)
  {
    GraphicsObjectHandle h = renderTargets[i];
    assert(h.IsValid());
    Graphics::RenderTargetResource* rt = GRAPHICS._renderTargets.Get(h);
    texture_desc = rt->texture.desc;
    if (!dsv && rt->dsv.resource)
    {
      dsv = rt->dsv.resource;
    }
    rts[i] = rt->rtv.resource;
    // clear render target (and depth stenci)
    if (clearTargets[i])
    {
      _ctx->ClearRenderTargetView(rts[i], &clearTargets[i].x);
      if (rt->dsv.resource)
      {
        _ctx->ClearDepthStencilView(rt->dsv.resource, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0 );
      }
    }
  }
  CD3D11_VIEWPORT viewport(0.0f, 0.0f, (float)texture_desc.Width, (float)texture_desc.Height);
  _ctx->RSSetViewports(1, &viewport);
  _ctx->OMSetRenderTargets(numRenderTargets, rts, dsv);
}

//------------------------------------------------------------------------------
void DeferredContext::GetRenderTargetTextureDesc(
    GraphicsObjectHandle handle,
    D3D11_TEXTURE2D_DESC* desc)
{
  auto rt = GRAPHICS._renderTargets.Get(
      handle.IsValid() ? handle : GRAPHICS._defaultRenderTarget);
  *desc = rt->texture.desc;
}

//------------------------------------------------------------------------------
void DeferredContext::SetSwapChain(GraphicsObjectHandle h, const float* clearColor)
{
  auto swapChain  = GRAPHICS._swapChains.Get(h);
  auto rt         = GRAPHICS._renderTargets.Get(swapChain->_renderTarget);
  _ctx->OMSetRenderTargets(1, &rt->rtv.resource.p, rt->dsv.resource);

  if (clearColor)
  {
    _ctx->ClearRenderTargetView(rt->rtv.resource, clearColor);
    _ctx->ClearDepthStencilView(rt->dsv.resource, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0 );
  }
  _ctx->RSSetViewports(1, &swapChain->_viewport);
}

//------------------------------------------------------------------------------
void DeferredContext::SetVS(GraphicsObjectHandle vs)
{
  assert(vs.type() == GraphicsObjectHandle::kVertexShader || !vs.IsValid());
  assert(GRAPHICS._vertexShaders.Get(vs));
  _ctx->VSSetShader(GRAPHICS._vertexShaders.Get(vs), NULL, 0);
}

//------------------------------------------------------------------------------
void DeferredContext::SetCS(GraphicsObjectHandle cs)
{
  assert(cs.type() == GraphicsObjectHandle::kComputeShader || !cs.IsValid());
  _ctx->CSSetShader(cs.IsValid() ? GRAPHICS._computeShaders.Get(cs) : NULL, NULL, 0);
}

//------------------------------------------------------------------------------
void DeferredContext::SetGS(GraphicsObjectHandle gs)
{
  assert(gs.type() == GraphicsObjectHandle::kGeometryShader || !gs.IsValid());
  _ctx->GSSetShader(gs.IsValid() ? GRAPHICS._geometryShaders.Get(gs) : NULL, NULL, 0);
}

//------------------------------------------------------------------------------
void DeferredContext::SetPS(GraphicsObjectHandle ps)
{
  assert(ps.type() == GraphicsObjectHandle::kPixelShader || !ps.IsValid());
  assert(GRAPHICS._pixelShaders.Get(ps));
  _ctx->PSSetShader(ps.IsValid() ? GRAPHICS._pixelShaders.Get(ps) : NULL, NULL, 0);
}

//------------------------------------------------------------------------------
void DeferredContext::SetLayout(GraphicsObjectHandle layout)
{
  if (layout.IsValid())
    _ctx->IASetInputLayout(GRAPHICS._inputLayouts.Get(layout));
  else
    _ctx->IASetInputLayout(nullptr);
}

//------------------------------------------------------------------------------
void DeferredContext::SetIB(GraphicsObjectHandle ib)
{
  _ctx->IASetIndexBuffer(GRAPHICS._indexBuffers.Get(ib), (DXGI_FORMAT)ib.data(), 0);
}

//------------------------------------------------------------------------------
void DeferredContext::SetIB(ID3D11Buffer *buf, DXGI_FORMAT format)
{
  _ctx->IASetIndexBuffer(buf, format, 0);
}

//------------------------------------------------------------------------------
void DeferredContext::SetVB(ID3D11Buffer *buf, u32 stride)
{
  UINT ofs[] = { 0 };
  ID3D11Buffer* bufs[] = { buf };
  u32 strides[] = { stride };
  _ctx->IASetVertexBuffers(0, 1, bufs, strides, ofs);
}

//------------------------------------------------------------------------------
void DeferredContext::SetVB(GraphicsObjectHandle vb) 
{
  SetVB(GRAPHICS._vertexBuffers.Get(vb), vb.data());
}

//------------------------------------------------------------------------------
void DeferredContext::SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY top)
{
  _ctx->IASetPrimitiveTopology(top);
}

//------------------------------------------------------------------------------
void DeferredContext::SetRasterizerState(GraphicsObjectHandle rs)
{
  _ctx->RSSetState(GRAPHICS._rasterizerStates.Get(rs));
}

//------------------------------------------------------------------------------
void DeferredContext::SetDepthStencilState(GraphicsObjectHandle dss, UINT stencil_ref)
{
  _ctx->OMSetDepthStencilState(GRAPHICS._depthStencilStates.Get(dss), stencil_ref);
}

//------------------------------------------------------------------------------
void DeferredContext::SetBlendState(GraphicsObjectHandle bs, const float* blendFactors, UINT sampleMask)
{
  _ctx->OMSetBlendState(GRAPHICS._blendStates.Get(bs), blendFactors, sampleMask);
}

//------------------------------------------------------------------------------
void DeferredContext::UnsetSRVs(u32 first, u32 count, ShaderType shaderType)
{
  ID3D11ShaderResourceView* srViews[16] = { nullptr };

  if (shaderType == ShaderType::VertexShader)
    _ctx->VSSetShaderResources(first, count, srViews);
  else if (shaderType == ShaderType::PixelShader)
    _ctx->PSSetShaderResources(first, count, srViews);
  else if (shaderType == ShaderType::ComputeShader)
    _ctx->CSSetShaderResources(first, count, srViews);
  else if (shaderType == ShaderType::GeometryShader)
    _ctx->GSSetShaderResources(first, count, srViews);
}

//------------------------------------------------------------------------------
void DeferredContext::UnsetUAVs(int first, int count)
{
  UINT initialCount = -1;
  static ID3D11UnorderedAccessView *nullViews[MAX_TEXTURES] = { nullptr };
  _ctx->CSSetUnorderedAccessViews(first, count, nullViews, &initialCount);
}

//------------------------------------------------------------------------------
void DeferredContext::UnsetRenderTargets(int first, int count)
{
  static ID3D11RenderTargetView *nullViews[8] = { nullptr };
  _ctx->OMSetRenderTargets(count, nullViews, nullptr);
}

//------------------------------------------------------------------------------
bool DeferredContext::Map(
  GraphicsObjectHandle h,
  UINT sub,
  D3D11_MAP type,
  UINT flags,
  D3D11_MAPPED_SUBRESOURCE *res)
{
  switch (h.type())
  {
  case GraphicsObjectHandle::kTexture:
    return SUCCEEDED(_ctx->Map(GRAPHICS._textures.Get(h)->texture.resource, sub, type, flags, res));

  case GraphicsObjectHandle::kVertexBuffer:
    return SUCCEEDED(_ctx->Map(GRAPHICS._vertexBuffers.Get(h), sub, type, flags, res));

  case GraphicsObjectHandle::kIndexBuffer:
    return SUCCEEDED(_ctx->Map(GRAPHICS._indexBuffers.Get(h), sub, type, flags, res));

  default:
    //LOG_ERROR_LN("Invalid resource type passed to %s", __FUNCTION__);
    return false;
  }
}

//------------------------------------------------------------------------------
void DeferredContext::Unmap(GraphicsObjectHandle h, UINT sub)
{
  switch (h.type())
  {
  case GraphicsObjectHandle::kTexture:
    _ctx->Unmap(GRAPHICS._textures.Get(h)->texture.resource, sub);
    break;

  case GraphicsObjectHandle::kVertexBuffer:
    _ctx->Unmap(GRAPHICS._vertexBuffers.Get(h), sub);
    break;

  case GraphicsObjectHandle::kIndexBuffer:
    _ctx->Unmap(GRAPHICS._indexBuffers.Get(h), sub);
    break;

  default:
    break;
    //LOG_WARNING_LN("Invalid resource type passed to %s", __FUNCTION__);
  }
}

//------------------------------------------------------------------------------
void DeferredContext::CopyToBuffer(GraphicsObjectHandle h, const void* data, u32 len)
{
  D3D11_MAPPED_SUBRESOURCE res;
  if (Map(h, 0, D3D11_MAP_WRITE_DISCARD, 0, &res))
  {
    memcpy(res.pData, data, len);
    Unmap(h, 0);
  }
}

//------------------------------------------------------------------------------
void DeferredContext::CopyToBuffer(
    GraphicsObjectHandle h,
    UINT sub,
    D3D11_MAP type,
    UINT flags,
    const void* data,
    u32 len)
{
  D3D11_MAPPED_SUBRESOURCE res;
  if (Map(h, 0, D3D11_MAP_WRITE_DISCARD, 0, &res))
  {
    memcpy(res.pData, data, len);
    Unmap(h, 0);
  }
}

//------------------------------------------------------------------------------
void DeferredContext::DrawIndexed(int count, int start_index, int base_vertex)
{
  _ctx->DrawIndexed(count, start_index, base_vertex);
}

//------------------------------------------------------------------------------
void DeferredContext::Draw(int vertexCount, int startVertexLocation)
{
  _ctx->Draw(vertexCount, startVertexLocation);
}

//------------------------------------------------------------------------------
void DeferredContext::Dispatch(int threadGroupCountX, int threadGroupCountY, int threadGroupCountZ)
{
  _ctx->Dispatch(threadGroupCountX, threadGroupCountY, threadGroupCountZ);
}

//------------------------------------------------------------------------------
void DeferredContext::BeginFrame()
{
}

//------------------------------------------------------------------------------
void DeferredContext::EndFrame()
{
  if (!_is_immediate_context)
  {
    ID3D11CommandList *cmd_list;
    if (SUCCEEDED(_ctx->FinishCommandList(FALSE, &cmd_list)))
      GRAPHICS.AddCommandList(cmd_list);
  }
}

//------------------------------------------------------------------------------
void DeferredContext::Flush()
{
  _ctx->Flush();
}

//------------------------------------------------------------------------------
void DeferredContext::SetShaderResources(
    const vector<GraphicsObjectHandle>& handles,
    ShaderType shaderType)
{
  ID3D11ShaderResourceView** v = (ID3D11ShaderResourceView**)_alloca(sizeof(ID3D11ShaderResourceView*) * handles.size());
  ID3D11ShaderResourceView** t = v;
  for (auto h : handles)
  {
    *t++ = GRAPHICS.GetShaderResourceView(h);
  }

  u32 count = (u32)handles.size();
  if (shaderType == ShaderType::VertexShader)
    _ctx->VSSetShaderResources(0, count, v);
  else if (shaderType == ShaderType::PixelShader)
    _ctx->PSSetShaderResources(0, count, v);
  else if (shaderType == ShaderType::ComputeShader)
    _ctx->CSSetShaderResources(0, count, v);
  else if (shaderType == ShaderType::GeometryShader)
    _ctx->GSSetShaderResources(0, count, v);
  else
    assert(false);

}

//------------------------------------------------------------------------------
void DeferredContext::SetShaderResource(GraphicsObjectHandle h, ShaderType shaderType)
{
  ID3D11ShaderResourceView* view = view = GRAPHICS.GetShaderResourceView(h);
  view = GRAPHICS.GetShaderResourceView(h);
  if (!view)
    return;

  if (shaderType == ShaderType::VertexShader)
    _ctx->VSSetShaderResources(0, 1, &view);
  else if (shaderType == ShaderType::PixelShader)
    _ctx->PSSetShaderResources(0, 1, &view);
  else if (shaderType == ShaderType::ComputeShader)
    _ctx->CSSetShaderResources(0, 1, &view);
  else if (shaderType == ShaderType::GeometryShader)
    _ctx->GSSetShaderResources(0, 1, &view);
  else
    assert(false);
    //LOG_ERROR_LN("Implement me!");
}

//------------------------------------------------------------------------------
void DeferredContext::SetUav(GraphicsObjectHandle h, Color* clearColor)
{
  auto type = h.type();

  ID3D11UnorderedAccessView* view = nullptr;
  if (type == GraphicsObjectHandle::kStructuredBuffer)
  {
    Graphics::StructuredBuffer* buf = GRAPHICS._structuredBuffers.Get(h);
    view = buf->uav.resource;
  }
  else if (type == GraphicsObjectHandle::kRenderTarget)
  {
    Graphics::RenderTargetResource* res = GRAPHICS._renderTargets.Get(h);
    view = res->uav.resource;

    if (clearColor)
    {
      _ctx->ClearRenderTargetView(res->rtv.resource, &clearColor->x);
    }
  }
  else
  {
    LOG_WARN("Trying to set an unsupported UAV type!");
    return;
  }

  u32 initialCount = 0;
  _ctx->CSSetUnorderedAccessViews(0, 1, &view, &initialCount);
}

//------------------------------------------------------------------------------
void DeferredContext::SetSamplerState(GraphicsObjectHandle h, u32 slot, ShaderType shaderType)
{
  ID3D11SamplerState* samplerState = GRAPHICS._sampler_states.Get(h);

  if (shaderType == ShaderType::VertexShader)
    _ctx->VSSetSamplers(slot, 1, &samplerState);
  else if (shaderType == ShaderType::PixelShader)
    _ctx->PSSetSamplers(slot, 1, &samplerState);
  else if (shaderType == ShaderType::ComputeShader)
    _ctx->CSSetSamplers(slot, 1, &samplerState);
  else if (shaderType == ShaderType::GeometryShader)
    _ctx->GSSetSamplers(slot, 1, &samplerState);
}

//------------------------------------------------------------------------------
void DeferredContext::SetSamplers(
    const GraphicsObjectHandle* h,
    u32 slot,
    u32 numSamplers,
    ShaderType shaderType)
{
  vector<ID3D11SamplerState*> samplers;
  for (u32 i = 0; i < numSamplers; ++i)
    samplers.push_back(GRAPHICS._sampler_states.Get(h[i]));

  if (shaderType == ShaderType::VertexShader)
    _ctx->VSSetSamplers(slot, numSamplers, samplers.data());
  else if (shaderType == ShaderType::PixelShader)
    _ctx->PSSetSamplers(slot, numSamplers, samplers.data());
  else if (shaderType == ShaderType::ComputeShader)
    _ctx->CSSetSamplers(slot, numSamplers, samplers.data());
  else if (shaderType == ShaderType::GeometryShader)
    _ctx->GSSetSamplers(slot, numSamplers, samplers.data());
}

//------------------------------------------------------------------------------
void DeferredContext::SetCBuffer(
    GraphicsObjectHandle h,
    const void* buf,
    size_t len,
    ShaderType shaderType,
    u32 slot)
{
  ID3D11Buffer *buffer = GRAPHICS._constantBuffers.Get(h);
  D3D11_MAPPED_SUBRESOURCE sub;
  if (SUCCEEDED(_ctx->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &sub)))
  {
    memcpy(sub.pData, buf, len);
    _ctx->Unmap(buffer, 0);
  }

  if (shaderType == ShaderType::VertexShader)
    _ctx->VSSetConstantBuffers(slot, 1, &buffer);
  else if (shaderType == ShaderType::PixelShader)
    _ctx->PSSetConstantBuffers(slot, 1, &buffer);
  else if (shaderType == ShaderType::ComputeShader)
    _ctx->CSSetConstantBuffers(slot, 1, &buffer);
  else if (shaderType == ShaderType::GeometryShader)
    _ctx->GSSetConstantBuffers(slot, 1, &buffer);
}

//------------------------------------------------------------------------------
void DeferredContext::SetGpuObjects(const GpuObjects& obj)
{
  if (obj._vs.IsValid()) SetVS(obj._vs);
  if (obj._ps.IsValid()) SetPS(obj._ps);

  if (obj._layout.IsValid()) SetLayout(obj._layout);
  if (obj._vb.IsValid()) SetVB(obj._vb);
  if (obj._ib.IsValid()) SetIB(obj._ib);

  SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

//------------------------------------------------------------------------------
void DeferredContext::SetGpuState(const GpuState& state)
{
  float blendFactor[4] ={ 1, 1, 1, 1 };
  SetRasterizerState(state._rasterizerState);
  SetBlendState(state._blendState, blendFactor, 0xffffffff);
  SetDepthStencilState(state._depthStencilState, 0);
}

//------------------------------------------------------------------------------
void DeferredContext::SetGpuStateSamplers(const GpuState& state, ShaderType shaderType)
{
  SetSamplers(state._samplers, 0, 4, shaderType);
}

//------------------------------------------------------------------------------
void DeferredContext::SetViewports(const D3D11_VIEWPORT& viewport, u32 numViewports)
{
  vector<D3D11_VIEWPORT> viewports(numViewports, viewport);
  _ctx->RSSetViewports(numViewports, viewports.data());
}
