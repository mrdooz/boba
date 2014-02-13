#pragma once
#include "graphics_object_handle.hpp"

namespace boba
{
  class DeferredContext
  {
    friend class Graphics;
  public:

    void GetRenderTargetTextureDesc(GraphicsObjectHandle handle, D3D11_TEXTURE2D_DESC* desc);
    void SetSwapChain(GraphicsObjectHandle h, bool clear);
    void SetRenderTarget(GraphicsObjectHandle render_target, bool clear_target);
    void SetRenderTargets(GraphicsObjectHandle *render_targets, bool *clear_targets, int num_render_targets);
    void GenerateMips(GraphicsObjectHandle h);

    void BeginFrame();
    void EndFrame();

    void SetVb(ID3D11Buffer *buf, uint32_t stride);
    void SetVs(GraphicsObjectHandle vs);
    void SetPs(GraphicsObjectHandle ps);
    void SetCs(GraphicsObjectHandle cs);
    void SetGs(GraphicsObjectHandle cs);
    void SetLayout(GraphicsObjectHandle layout);
    void SetVb(GraphicsObjectHandle vb);
    void SetIb(GraphicsObjectHandle ib);
    void SetTopology(D3D11_PRIMITIVE_TOPOLOGY top);
    void SetRs(GraphicsObjectHandle rs);
    void SetDss(GraphicsObjectHandle dss, UINT stencil_ref);
    void SetBs(GraphicsObjectHandle bs, const float *blend_factors, UINT sample_mask);
    void UnsetUavs(int first, int count);
    void UnsetRenderTargets(int first, int count);
    void DrawIndexed(int count, int start_index, int base_vertex);
    void Draw(int vertexCount, int startVertexLocation);
    void Dispatch(int threadGroupCountX, int threadGroupCountY, int threadGroupCountZ);

    bool Map(GraphicsObjectHandle h, UINT sub, D3D11_MAP type, UINT flags, D3D11_MAPPED_SUBRESOURCE *res);
    void Unmap(GraphicsObjectHandle h, UINT sub);

    void Flush();

    //void set_samplers(const SamplerArray &samplers);
    //void SetShaderResources(const TextureArray &resources, ShaderType type);
    //void SetShaderResource(GraphicsObjectHandle resource, ShaderType type);
    //void set_uavs(const TextureArray &uavs);
    //void unset_shader_resource(int first_view, int num_views, ShaderType type);
    //void SetCBuffers(const vector<CBuffer *> &vs, const vector<CBuffer *> &ps);
    //void SetCBuffer(const CBuffer &vs, const CBuffer &ps);
    //void SetCBuffer(GraphicsObjectHandle cb, int slot, ShaderType type, const void *data, int dataLen);
    //void FillSystemResourceViews(const ResourceViewArray &views, TextureArray *out) const;

  private:
    DeferredContext();
    ID3D11DeviceContext *_ctx;

    u32 _default_stencil_ref;
    float _default_blend_factors[4];
    u32 _default_sample_mask;

    bool _is_immediate_context;
  };
}
