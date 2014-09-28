#include "particle_test.hpp"
#include "graphics.hpp"
#include "deferred_context.hpp"
#include "app.hpp"
#include "demo_engine.hpp"
#include "proto_helpers.hpp"
#include "resource_manager.hpp"

#if WITH_ANT_TWEAK_BAR
#include "protocol/effect_settings_particle_bindings.hpp"
#endif


using namespace boba;

namespace
{
  struct CBufferPerFrame
  {
    Vector4 col;
  };
}
//------------------------------------------------------------------------------
ParticleTest::ParticleTest(const string &name, u32 id)
  : Effect(name, id)
{
}

//------------------------------------------------------------------------------
ParticleTest::~ParticleTest()
{
}

//------------------------------------------------------------------------------
bool ParticleTest::Show()
{
  return true;
}

//------------------------------------------------------------------------------
bool ParticleTest::Hide()
{
  return true;
}

//------------------------------------------------------------------------------
bool ParticleTest::Init(const protocol::effect::EffectSetting& config)
{
  _config = config.particle_config();
//   _configName = config;
//   if (!LoadProto(config, &_config))
//     return false;

#if WITH_ANT_TWEAK_BAR
  BindParticleConfig(&_config, nullptr);
  TwDefine("particle.Config visible=false");
#endif

  _texture = RESOURCE_MANAGER.LoadTexture("gfx/Abstract_BG_Texture2.jpg");
  if (!_texture.IsValid())
    return false;

  if (!GRAPHICS.LoadShadersFromFile("shaders/fullscreen", &_vs, &_ps, nullptr, 0))
    return false;

  CD3D11_SAMPLER_DESC sampler = CD3D11_SAMPLER_DESC(CD3D11_DEFAULT());
  _samplerState = GRAPHICS.CreateSamplerState(sampler);
  if (!_samplerState.IsValid())
    return false;

  _cbuffer = GRAPHICS.CreateBuffer(D3D11_BIND_CONSTANT_BUFFER, sizeof(CBufferPerFrame), true);
  if (!_cbuffer.IsValid())
    return false;

  return true;
}

//------------------------------------------------------------------------------
bool ParticleTest::Update(const UpdateState& state)
{
  return true;
}

//------------------------------------------------------------------------------
bool ParticleTest::Render()
{
  float black[] ={ 0, 0, 0, 0 };
  _ctx->SetSwapChain(GRAPHICS.DefaultSwapChain(), black);
  _ctx->BeginFrame();

  CBufferPerFrame cb;
  ::boba::common::FromProtocol(_config.bb_col4f(), &cb.col);
  _ctx->SetCBuffer(_cbuffer, &cb, sizeof(cb), ShaderType::PixelShader, 0);

  _ctx->SetVS(_vs);
  _ctx->SetPS(_ps);
  _ctx->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  _ctx->SetShaderResource(_texture, ShaderType::PixelShader);
  _ctx->SetSamplerState(_samplerState, 0, ShaderType::PixelShader);
  _ctx->Draw(3,0);

  _ctx->EndFrame();

  return true;
}

//------------------------------------------------------------------------------
bool ParticleTest::Close()
{
  return true;
}

//------------------------------------------------------------------------------
bool ParticleTest::SaveSettings()
{
  if (FILE* f = fopen(_configName.c_str() ,"wt"))
  {
    fprintf(f, "%s", _config.DebugString().c_str());
    fclose(f);
  }
  return true;
}

//------------------------------------------------------------------------------
Effect* ParticleTest::Create(const char* name, u32 id)
{
  return new ParticleTest(name, id);
}

//------------------------------------------------------------------------------
const char* ParticleTest::Name()
{
  return "particle_test";
}
