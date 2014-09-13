#include "post_process.hpp"
#include "deferred_context.hpp"
#include "init_sequence.hpp"

using namespace boba;

//------------------------------------------------------------------------------
PostProcess::PostProcess(DeferredContext* ctx)
  : _ctx(ctx)
{
}

//------------------------------------------------------------------------------
bool PostProcess::Init()
{
  BEGIN_INIT_SEQUENCE();

  INIT(_cb.Create());

  CD3D11_DEPTH_STENCIL_DESC dsDesc = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT());
  dsDesc.DepthEnable = FALSE;
  dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
  dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

  CD3D11_RASTERIZER_DESC rsDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT());
  rsDesc.CullMode = D3D11_CULL_NONE;

  INIT(_gpuState.Create(dsDesc, CD3D11_BLEND_DESC(CD3D11_DEFAULT()), rsDesc));

  INIT(GRAPHICS.LoadShadersFromFile("shaders/quad", &_vsQuad, nullptr, nullptr, 0));

  END_INIT_SEQUENCE();
}

//------------------------------------------------------------------------------
void PostProcess::Setup()
{
  _ctx->SetGpuState(_gpuState);
  _ctx->SetGpuStateSamplers(_gpuState, ShaderType::PixelShader);
  _ctx->SetGpuStateSamplers(_gpuState, ShaderType::ComputeShader);

  _ctx->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  _ctx->SetVS(_vsQuad);
  _ctx->SetGS(ObjectHandle());
  _ctx->SetIB(nullptr, DXGI_FORMAT_R32_UINT);
  _ctx->SetVB(nullptr, 24);
}

//------------------------------------------------------------------------------
void PostProcess::Execute(
  const vector<ObjectHandle>& input,
  ObjectHandle output,
  ObjectHandle shader,
  const Color* clearColor,
  WCHAR* name)
{
  GPU_BeginEvent(0xffffffff, name);

  _ctx->SetLayout(ObjectHandle());

  if (output.IsValid())
    _ctx->SetRenderTarget(output, clearColor);

  _ctx->SetShaderResources(input, ShaderType::PixelShader);

  // set constant buffers
  u32 inputX[8], inputY[8];
  u32 outputX, outputY;

  for (size_t i = 0; i < input.size(); ++i)
    GRAPHICS.GetTextureSize(input[i], &inputX[i], &inputY[i]);
  GRAPHICS.GetTextureSize(output, &outputX, &outputY);
  _cb.data.inputSize.x = (float)inputX[0];
  _cb.data.inputSize.y = (float)inputY[0];
  _cb.data.outputSize.x = (float)outputX;
  _cb.data.outputSize.y = (float)outputY;
  _ctx->SetCBuffer(_cb, ShaderType::PixelShader, 0);

  CD3D11_VIEWPORT viewport = CD3D11_VIEWPORT(0.f, 0.f, (float)outputX, (float)outputY);
  _ctx->SetViewports(viewport, 1);

  _ctx->SetPS(shader);
  _ctx->Draw(6, 0);

  if (output.IsValid())
    _ctx->UnsetRenderTargets(0, 1);

  _ctx->UnsetSRVs(0, (u32)input.size(), ShaderType::PixelShader);

  GPU_EndEvent();
}
