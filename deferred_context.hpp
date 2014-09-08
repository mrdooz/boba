#pragma once
#include "graphics_object_handle.hpp"
#include "graphics.hpp"
#include "gpu_objects.hpp"

namespace boba
{
  class DeferredContext
  {
    friend class Graphics;
  public:

    void GetRenderTargetTextureDesc(GraphicsObjectHandle handle, D3D11_TEXTURE2D_DESC* desc);
    void SetSwapChain(GraphicsObjectHandle h, const float* clearColor);
    void SetRenderTarget(GraphicsObjectHandle render_target, const Color* clearTarget);
    void SetRenderTargets(GraphicsObjectHandle *render_targets, const Color* clearTarget, int num_render_targets);
    void GenerateMips(GraphicsObjectHandle h);

    void BeginFrame();
    void EndFrame();

    void SetVB(ID3D11Buffer *buf, uint32_t stride);
    void SetIB(ID3D11Buffer *buf, DXGI_FORMAT format);
    void SetVS(GraphicsObjectHandle vs);
    void SetPS(GraphicsObjectHandle ps);
    void SetCS(GraphicsObjectHandle cs);
    void SetGS(GraphicsObjectHandle cs);
    void SetLayout(GraphicsObjectHandle layout);
    void SetVB(GraphicsObjectHandle vb);
    void SetIB(GraphicsObjectHandle ib);
    void SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY top);
    void SetRasterizerState(GraphicsObjectHandle rs);
    void SetDepthStencilState(GraphicsObjectHandle dss, UINT stencil_ref);
    void SetBlendState(GraphicsObjectHandle bs, const float *blendFactors, UINT sampleMask);
    void SetShaderResource(GraphicsObjectHandle h, ShaderType shaderType);
    void SetShaderResources(const vector<GraphicsObjectHandle>& handles, ShaderType shaderType);
    void SetUav(GraphicsObjectHandle h, Color* clearColor);
    void SetSamplerState(GraphicsObjectHandle h, u32 slot, ShaderType shaderType);
    void SetSamplers(const GraphicsObjectHandle* h, u32 slot, u32 numSamplers, ShaderType shaderType);
    template <typename T>
    void SetCBuffer(const ConstantBuffer<T>& buffer, ShaderType shaderType, u32 slot)
    {
      return SetCBuffer(buffer.handle, &buffer.data, sizeof(T), shaderType, slot);
    }

    void SetCBuffer(GraphicsObjectHandle h, const void* buf, size_t len, ShaderType shaderType, u32 slot);
    void SetGpuObjects(const GpuObjects& obj);
    void SetGpuState(const GpuState& state);
    void SetGpuStateSamplers(const GpuState& state, ShaderType shaderType);
    void SetViewports(const D3D11_VIEWPORT& viewport, u32 numViewports);

    void UnsetSRVs(u32 first, u32 count, ShaderType shaderType);
    void UnsetUAVs(int first, int count);
    void UnsetRenderTargets(int first, int count);
    void DrawIndexed(int count, int start_index, int base_vertex);
    void Draw(int vertexCount, int startVertexLocation);
    void Dispatch(int threadGroupCountX, int threadGroupCountY, int threadGroupCountZ);

    bool Map(GraphicsObjectHandle h, UINT sub, D3D11_MAP type, UINT flags, D3D11_MAPPED_SUBRESOURCE *res);
    void Unmap(GraphicsObjectHandle h, UINT sub);
    void CopyToBuffer(GraphicsObjectHandle h, UINT sub, D3D11_MAP type, UINT flags, const void* data, u32 len);
    void CopyToBuffer(GraphicsObjectHandle h, const void* data, u32 len);

    void Flush();

  private:
    DeferredContext();
    ID3D11DeviceContext *_ctx;

    u32 _default_stencil_ref;
    float _default_blend_factors[4];
    u32 _default_sample_mask;

    bool _is_immediate_context;
  };
}
