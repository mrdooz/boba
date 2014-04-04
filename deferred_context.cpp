#include "graphics.hpp"
#include "deferred_context.hpp"

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
  _ctx->GenerateMips(GRAPHICS._renderTargets.Get(h)->srv.resource);
}

//------------------------------------------------------------------------------
void DeferredContext::SetRenderTarget(GraphicsObjectHandle render_target, bool clear_target)
{
  SetRenderTargets(&render_target, &clear_target, 1);
}

//------------------------------------------------------------------------------
void DeferredContext::SetRenderTargets(
    GraphicsObjectHandle *render_targets,
    bool *clear_targets,
    int num_render_targets)
{

  ID3D11RenderTargetView *rts[8] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
  ID3D11DepthStencilView *dsv = nullptr;
  D3D11_TEXTURE2D_DESC texture_desc;
  float color[4] = {0,0,0,0};

  // Collect the valid render targets, set the first available depth buffer
  // and clear targets if specified
  for (int i = 0; i < num_render_targets; ++i)
  {
    GraphicsObjectHandle h = render_targets[i];
    assert(h.IsValid());
    Graphics::RenderTargetResource* rt = GRAPHICS._renderTargets.Get(h);
    texture_desc = rt->texture.desc;
    if (!dsv && rt->dsv.resource)
    {
      dsv = rt->dsv.resource;
    }
    rts[i] = rt->rtv.resource;
    // clear render target (and depth stenci)
    if (clear_targets[i])
    {
      _ctx->ClearRenderTargetView(rts[i], color);
      if (rt->dsv.resource)
      {
        _ctx->ClearDepthStencilView(rt->dsv.resource, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0 );
      }
    }
  }
  CD3D11_VIEWPORT viewport(0.0f, 0.0f, (float)texture_desc.Width, (float)texture_desc.Height);
  _ctx->RSSetViewports(1, &viewport);
  _ctx->OMSetRenderTargets(num_render_targets, rts, dsv);
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
void DeferredContext::SetSwapChain(GraphicsObjectHandle h, bool clear)
{
  Graphics& g = GRAPHICS;
  auto swapChain  = g._swapChains.Get(h);
  auto rt         = g._renderTargets.Get(swapChain->_renderTarget);
  _ctx->OMSetRenderTargets(1, &rt->rtv.resource.p, rt->dsv.resource);
  if (clear)
  {
    static float color[4] = {0,0,0,0};
    _ctx->ClearRenderTargetView(rt->rtv.resource, color);
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
  _ctx->IASetInputLayout(GRAPHICS._inputLayouts.Get(layout));
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
void DeferredContext::SetIB(GraphicsObjectHandle ib)
{
  _ctx->IASetIndexBuffer(GRAPHICS._indexBuffers.Get(ib), (DXGI_FORMAT)ib.data(), 0);
}

//------------------------------------------------------------------------------
void DeferredContext::SetTopology(D3D11_PRIMITIVE_TOPOLOGY top)
{
  _ctx->IASetPrimitiveTopology(top);
}

//------------------------------------------------------------------------------
void DeferredContext::SetRS(GraphicsObjectHandle rs)
{
  _ctx->RSSetState(GRAPHICS._rasterizerStates.Get(rs));
}

//------------------------------------------------------------------------------
void DeferredContext::SetDSS(GraphicsObjectHandle dss, UINT stencil_ref)
{
  _ctx->OMSetDepthStencilState(GRAPHICS._depthStencilStates.Get(dss), stencil_ref);
}

//------------------------------------------------------------------------------
void DeferredContext::SetBS(GraphicsObjectHandle bs, const float *blend_factors, UINT sample_mask)
{
  _ctx->OMSetBlendState(GRAPHICS._blendStates.Get(bs), blend_factors, sample_mask);
}

//------------------------------------------------------------------------------
void DeferredContext::UnsetUavs(int first, int count)
{
  UINT initialCount = -1;
  static ID3D11UnorderedAccessView *nullViews[MAX_TEXTURES] = {0, 0, 0, 0, 0, 0, 0, 0};
  _ctx->CSSetUnorderedAccessViews(first, count, nullViews, &initialCount);
}

//------------------------------------------------------------------------------
void DeferredContext::UnsetRenderTargets(int first, int count)
{
  static ID3D11RenderTargetView *nullViews[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  _ctx->OMSetRenderTargets(count, &nullViews[first], nullptr);
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
    _ctx->FinishCommandList(FALSE, &cmd_list);
    GRAPHICS.AddCommandList(cmd_list);
  }
}

//------------------------------------------------------------------------------
void DeferredContext::Flush()
{
  _ctx->Flush();
}

//------------------------------------------------------------------------------
void DeferredContext::SetShaderResource(GraphicsObjectHandle h, ShaderType shaderType)
{
  GraphicsObjectHandle::Type type = h.type();
  ID3D11ShaderResourceView* view = nullptr;

  if (type == GraphicsObjectHandle::kTexture)
  {
    view = GRAPHICS._textures.Get(h)->view.resource;
  }
  else if (type == GraphicsObjectHandle::kResource)
  {
    view = GRAPHICS._resources.Get(h)->view.resource;
  }
  else if (type == GraphicsObjectHandle::kRenderTarget)
  {
    view = GRAPHICS._renderTargets.Get(h)->srv.resource;
  }
  else if (type == GraphicsObjectHandle::kStructuredBuffer)
  {
    view = GRAPHICS._structuredBuffers.Get(h)->srv.resource;
  }
  else
  {
    //LOG_ERROR_LN("Trying to set invalid resource type!");
  }

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
void DeferredContext::SetSamplerState(GraphicsObjectHandle h, ShaderType shaderType)
{
  ID3D11SamplerState* samplerState = GRAPHICS._sampler_states.Get(h);

  if (shaderType == ShaderType::VertexShader)
    _ctx->VSSetSamplers(0, 1, &samplerState);
  else if (shaderType == ShaderType::PixelShader)
    _ctx->PSSetSamplers(0, 1, &samplerState);
  else if (shaderType == ShaderType::ComputeShader)
    _ctx->CSSetSamplers(0, 1, &samplerState);
  else if (shaderType == ShaderType::GeometryShader)
    _ctx->GSSetSamplers(0, 1, &samplerState);
}

//------------------------------------------------------------------------------
void DeferredContext::SetCBuffer(
    GraphicsObjectHandle h,
    const void* buf,
    size_t len,
    ShaderType shaderType)
{
  ID3D11Buffer *buffer = GRAPHICS._constantBuffers.Get(h);
  D3D11_MAPPED_SUBRESOURCE sub;
  _ctx->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &sub);
  memcpy(sub.pData, buf, len);
  _ctx->Unmap(buffer, 0);

  if (shaderType == ShaderType::VertexShader)
    _ctx->VSSetConstantBuffers(0, 1, &buffer);
  else if (shaderType == ShaderType::PixelShader)
    _ctx->PSSetConstantBuffers(0, 1, &buffer);
  else if (shaderType == ShaderType::ComputeShader)
    _ctx->CSSetConstantBuffers(0, 1, &buffer);
  else if (shaderType == ShaderType::GeometryShader)
    _ctx->GSSetConstantBuffers(0, 1, &buffer);
}

//------------------------------------------------------------------------------
/*
void DeferredContext::set_samplers(const SamplerArray &samplers)
{
  int size = samplers.size() * sizeof(GraphicsObjectHandle);
  int first_sampler = MAX_SAMPLERS, num_samplers = 0;
  ID3D11SamplerState *d3dsamplers[MAX_SAMPLERS];
  for (int i = 0; i < MAX_SAMPLERS; ++i)
  {
    if (samplers[i].is_valid())
    {
      d3dsamplers[i] = GRAPHICS._sampler_states.Get(samplers[i], BufferType::FrontBuffer);
      first_sampler = min(first_sampler, i);
      num_samplers++;
    }
  }

  if (num_samplers)
  {
    _ctx->PSSetSamplers(first_sampler, num_samplers, &d3dsamplers[first_sampler]);
  }
}
*/

/*
void DeferredContext::set_uavs(const TextureArray &uavs)
{

  int size = uavs.size() * sizeof(GraphicsObjectHandle);
  ID3D11UnorderedAccessView *d3dUavs[MAX_TEXTURES];
  int first_resource = MAX_TEXTURES, num_resources = 0;

  for (int i = 0; i < MAX_TEXTURES; ++i)
  {

    if (uavs[i].is_valid())
    {
      GraphicsObjectHandle h = uavs[i];
      auto type = h.type();

      if (type == GraphicsObjectHandle::kStructuredBuffer)
      {
        auto *data = GRAPHICS._structured_buffers.Get(h, BufferType::FrontBuffer);
        d3dUavs[i] = data->uav.resource;
      } 
      else if (type == GraphicsObjectHandle::kRenderTarget)
      {
        auto *data = GRAPHICS._render_targets.Get(h, BufferType::FrontBuffer);
        d3dUavs[i] = data->uav.resource;
      }
      else
      {
        LOG_ERROR_LN("Trying to set an unsupported UAV type!");
      }
      num_resources++;
      first_resource = min(first_resource, i);
    }
  }

  if (num_resources) {
    int ofs = first_resource;
    int num_resources_set = 0;
    while (ofs < MAX_TEXTURES && num_resources_set < num_resources) {
      // handle non sequential resources
      int cur = 0;
      int tmp = ofs;
      while (uavs[ofs].is_valid()) {
        ofs++;
        cur++;
        if (ofs == MAX_TEXTURES)
          break;
      }
      UINT initialCount = 0;
      _ctx->CSSetUnorderedAccessViews(tmp, cur, &d3dUavs[tmp], &initialCount);
      num_resources_set += cur;
      while (ofs < MAX_TEXTURES && !uavs[ofs].is_valid())
        ofs++;
    }
  }
}

void DeferredContext::SetShaderResource(GraphicsObjectHandle resource, ShaderType type) {
  TextureArray arr = { resource };
  return SetShaderResources(arr, type);
}

void DeferredContext::SetShaderResources(const TextureArray &resources, ShaderType type)
{
  int size = resources.size() * sizeof(GraphicsObjectHandle);
  ID3D11ShaderResourceView *d3dresources[MAX_TEXTURES];
  int first_resource = MAX_TEXTURES, num_resources = 0;

  for (int i = 0; i < MAX_TEXTURES; ++i)
  {
    if (resources[i].is_valid())
    {
      GraphicsObjectHandle h = resources[i];
      auto type = h.type();
      if (type == GraphicsObjectHandle::kTexture)
      {
        auto *data = GRAPHICS._textures.Get(h, BufferType::FrontBuffer);
        d3dresources[i] = data->view.resource;
      }
      else if (type == GraphicsObjectHandle::kResource)
      {
        auto *data = GRAPHICS._resources.Get(h, BufferType::FrontBuffer);
        d3dresources[i] = data->view.resource;
      }
      else if (type == GraphicsObjectHandle::kRenderTarget)
      {
        auto *data = GRAPHICS._render_targets.Get(h, BufferType::FrontBuffer);
        d3dresources[i] = data->srv.resource;
      }
      else if (type == GraphicsObjectHandle::kStructuredBuffer)
      {
        auto *data = GRAPHICS._structured_buffers.Get(h, BufferType::FrontBuffer);
        d3dresources[i] = data->srv.resource;
      }
      else
      {
        LOG_ERROR_LN("Trying to set invalid resource type!");
      }
      num_resources++;
      first_resource = min(first_resource, i);
    }
  }

  if (num_resources) {
    int ofs = first_resource;
    int num_resources_set = 0;
    while (ofs < MAX_TEXTURES && num_resources_set < num_resources) {
      // handle non sequential resources
      int cur = 0;
      int tmp = ofs;
      while (resources[ofs].is_valid()) {
        ofs++;
        cur++;
        if (ofs == MAX_TEXTURES)
          break;
      }

      if (type == ShaderType::kVertexShader)
        _ctx->VSSetShaderResources(tmp, cur, &d3dresources[tmp]);
      else if (type == ShaderType::kPixelShader)
        _ctx->PSSetShaderResources(tmp, cur, &d3dresources[tmp]);
      else if (type == ShaderType::kComputeShader)
        _ctx->CSSetShaderResources(tmp, cur, &d3dresources[tmp]);
      else if (type == ShaderType::kGeometryShader)
        _ctx->GSSetShaderResources(tmp, cur, &d3dresources[tmp]);
      else
        LOG_ERROR_LN("Implement me!");

      num_resources_set += cur;
      while (ofs < MAX_TEXTURES && !resources[ofs].is_valid())
        ofs++;
    }
  }
}
*/
/*
void DeferredContext::unset_shader_resource(int first_view, int num_views, ShaderType type)
{
  if (!num_views)
    return;
  static ID3D11ShaderResourceView *null_views[MAX_SAMPLERS] = {0, 0, 0, 0, 0, 0, 0, 0};
  if (type == ShaderType::kVertexShader)
    _ctx->VSSetShaderResources(first_view, num_views, null_views);
  else if (type == ShaderType::kPixelShader)
    _ctx->PSSetShaderResources(first_view, num_views, null_views);
  else if (type == ShaderType::kComputeShader)
    _ctx->CSSetShaderResources(first_view, num_views, null_views);
  else if (type == ShaderType::kGeometryShader)
    _ctx->GSSetShaderResources(first_view, num_views, null_views);
  else
    LOG_ERROR_LN("Implement me!");

}

void DeferredContext::SetCBuffer(const CBuffer &vs, const CBuffer &ps) {

  if (vs.staging.size() > 0) {
    ID3D11Buffer *buffer = GRAPHICS._constant_buffers.Get(vs.handle, BufferType::FrontBuffer);
    D3D11_MAPPED_SUBRESOURCE sub;
    _ctx->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &sub);
    memcpy(sub.pData, vs.staging.data(), vs.staging.size());
    _ctx->Unmap(buffer, 0);

    _ctx->VSSetConstantBuffers(vs.slot, 1, &buffer);
  }

  if (ps.staging.size() > 0) {
    ID3D11Buffer *buffer = GRAPHICS._constant_buffers.Get(ps.handle, BufferType::FrontBuffer);
    D3D11_MAPPED_SUBRESOURCE sub;
    _ctx->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &sub);
    memcpy(sub.pData, ps.staging.data(), ps.staging.size());
    _ctx->Unmap(buffer, 0);

    _ctx->PSSetConstantBuffers(ps.slot, 1, &buffer);
  }
}

void DeferredContext::SetCBuffers(const vector<CBuffer *> &vs, const vector<CBuffer *> &ps)
{
  ID3D11Buffer **vs_cb = (ID3D11Buffer **)_alloca(vs.size() * sizeof(ID3D11Buffer *));
  ID3D11Buffer **ps_cb = (ID3D11Buffer **)_alloca(ps.size() * sizeof(ID3D11Buffer *));

  // Copy the vs cbuffers
  int firstVsSlot = D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT;
  for (size_t i = 0; i < vs.size(); ++i) {
    if (auto *cur = vs[i]) {
      int slot = cur->slot;
      firstVsSlot = min(firstVsSlot, slot);
      ID3D11Buffer *buffer = GRAPHICS._constant_buffers.Get(cur->handle, BufferType::FrontBuffer);
      D3D11_MAPPED_SUBRESOURCE sub;
      _ctx->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &sub);
      memcpy(sub.pData, cur->staging.data(), cur->staging.size());
      _ctx->Unmap(buffer, 0);
      vs_cb[slot] = buffer;
    }
  }

  // Copy the ps cbuffers
  int firstPsSlot = D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT;
  for (size_t i = 0; i < ps.size(); ++i) {
    if (auto *cur = ps[i]) {
      int slot = cur->slot;
      firstPsSlot = min(firstPsSlot, slot);
      ID3D11Buffer *buffer = GRAPHICS._constant_buffers.Get(cur->handle, BufferType::FrontBuffer);
      D3D11_MAPPED_SUBRESOURCE sub;
      _ctx->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &sub);
      memcpy(sub.pData, cur->staging.data(), cur->staging.size());
      _ctx->Unmap(buffer, 0);
      ps_cb[slot] = buffer;
    }
  }

  if (!vs.empty())
    _ctx->VSSetConstantBuffers(firstVsSlot, vs.size() - firstVsSlot, &vs_cb[firstVsSlot]);

  if (!ps.empty())
      _ctx->PSSetConstantBuffers(firstPsSlot, ps.size() - firstPsSlot, &ps_cb[firstPsSlot]);
}

void DeferredContext::SetCBuffer(
    GraphicsObjectHandle cb,
    int slot,
    ShaderType type,
    const void *data,
    int dataLen)
{
  KASSERT(dataLen == cb.data());
  ID3D11Buffer *buffer = GRAPHICS._constant_buffers.Get(cb, BufferType::FrontBuffer);
  if (!buffer)
    return;
  D3D11_MAPPED_SUBRESOURCE sub;
  _ctx->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &sub);
  memcpy(sub.pData, data, dataLen);
  _ctx->Unmap(buffer, 0);

  if (type == ShaderType::kVertexShader)
    _ctx->VSSetConstantBuffers(slot, 1, &buffer);
  else if (type == ShaderType::kPixelShader)
    _ctx->PSSetConstantBuffers(slot, 1, &buffer);
  else if (type == ShaderType::kComputeShader)
    _ctx->CSSetConstantBuffers(slot, 1, &buffer);
  else if (type == ShaderType::kGeometryShader)
    _ctx->GSSetConstantBuffers(slot, 1, &buffer);
  else
    LOG_ERROR_LN("Implement me!");
}
*/
