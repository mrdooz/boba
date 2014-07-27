#include "post_process.hpp"
#include "deferred_context.hpp"

using namespace boba;

//------------------------------------------------------------------------------
PostProcess::PostProcess(DeferredContext* ctx)
  : _ctx(ctx)
{
}

//------------------------------------------------------------------------------
bool PostProcess::Init()
{
  _cb.Create();

  CD3D11_DEPTH_STENCIL_DESC dsDesc = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT());
  dsDesc.DepthEnable = FALSE;
  dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
  dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
  _depthStencilState = GRAPHICS.CreateDepthStencilState(dsDesc);

  _blendState = GRAPHICS.CreateBlendState(CD3D11_BLEND_DESC(CD3D11_DEFAULT()));

  CD3D11_RASTERIZER_DESC rsDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT());
  rsDesc.CullMode = D3D11_CULL_NONE;
  _rasterizerState = GRAPHICS.CreateRasterizerState(rsDesc);

  CD3D11_SAMPLER_DESC samplerDesc = CD3D11_SAMPLER_DESC(CD3D11_DEFAULT());
  _linearSamplerState = GRAPHICS.CreateSamplerState(samplerDesc);

  samplerDesc.AddressU = samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
  _linearWrapSamplerState = GRAPHICS.CreateSamplerState(samplerDesc);

  samplerDesc.AddressU = samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
  _linearBorderSamplerState = GRAPHICS.CreateSamplerState(samplerDesc);

  samplerDesc.AddressU = samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
  samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
  _pointSamplerState = GRAPHICS.CreateSamplerState(samplerDesc);

  GRAPHICS.LoadShadersFromFile("shaders/quad", &_vsQuad, nullptr, nullptr, 0);

  return true;
}

//------------------------------------------------------------------------------
void PostProcess::Setup()
{
  float blendFactor[4] ={ 1, 1, 1, 1 };
  _ctx->SetRasterizerState(_rasterizerState);
  _ctx->SetBlendState(_blendState, blendFactor, 0xffffffff);
  _ctx->SetDepthStencilState(_depthStencilState, 0);

  GraphicsObjectHandle samplers[] ={ _pointSamplerState, _linearSamplerState,
    _linearWrapSamplerState, _linearBorderSamplerState };
  _ctx->SetSamplers(samplers, 0, 4, ShaderType::PixelShader);
  _ctx->SetSamplers(samplers, 0, 4, ShaderType::ComputeShader);

  _ctx->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  _ctx->SetVS(_vsQuad);
  _ctx->SetGS(GraphicsObjectHandle());
}

//------------------------------------------------------------------------------
void PostProcess::Execute(
  const vector<GraphicsObjectHandle>& input,
  GraphicsObjectHandle output,
  GraphicsObjectHandle shader,
  const Color* clearColor,
  WCHAR* name)
{
  GPU_BeginEvent(0xffffffff, name);

  _ctx->SetLayout(GraphicsObjectHandle());

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
  _ctx->DrawIndexed(6, 0, 0);

  if (output.IsValid())
    _ctx->UnsetRenderTargets(0, 1);

  _ctx->UnsetSRVs(0, (u32)input.size(), ShaderType::PixelShader);

  GPU_EndEvent();
}
