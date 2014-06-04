#include "particle_test.hpp"
#include "graphics.hpp"
#include "deferred_context.hpp"
#include "app.hpp"
#include "demo_engine.hpp"
#include "proto_helpers.hpp"
#include "resource_manager.hpp"

#if WITH_ANT_TWEAK_BAR
#include "protocol/particle_bindings.hpp"
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
ParticleTest::ParticleTest(const string &name)
  : Effect(name)
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
bool ParticleTest::Init(const char* config)
{
  _configName = config;

  if (!LoadProto(config, &_config))
    return false;

#if WITH_ANT_TWEAK_BAR
  BindConfig(&_config, nullptr);
  TwDefine("particle.Config visible=false");
#endif

  _texture = RESOURCE_MANAGER.LoadTexture("gfx/Abstract_BG_Texture2.jpg");
  if (!_texture.IsValid())
    return false;

  vector<char> buf;
  if (!RESOURCE_MANAGER.LoadFile("shaders/fullscreen.vso", &buf))
    return false;

  _vs = GRAPHICS.CreateVertexShader(buf, "VSQuad");
  if (!_vs.IsValid())
    return false;

  if (!RESOURCE_MANAGER.LoadFile("shaders/fullscreen.pso", &buf))
    return false;

  _ps = GRAPHICS.CreatePixelShader(buf, "PSQuad");
  if (!_ps.IsValid())
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
  _ctx->SetSwapChain(GRAPHICS.DefaultSwapChain(), true);
  _ctx->BeginFrame();

  CBufferPerFrame cb;
  FromProtocol(_config.bb_col4f(), &cb.col);
  _ctx->SetCBuffer(_cbuffer, &cb, sizeof(cb), ShaderType::PixelShader);

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
Effect* ParticleTest::Create(const char* name)
{
  return new ParticleTest(name);
}

//------------------------------------------------------------------------------
const char* ParticleTest::Name()
{
  return "particle_test";
}
