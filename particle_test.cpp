#include "particle_test.hpp"
#include "graphics.hpp"
#include "deferred_context.hpp"
#include "app.hpp"
#include "demo_engine.hpp"
#include "proto_helpers.hpp"

using namespace boba;

//------------------------------------------------------------------------------
ParticleTest::ParticleTest(const string &name, GraphicsObjectHandle swapChain)
  : Effect(name, swapChain)
{
}

//------------------------------------------------------------------------------
ParticleTest::~ParticleTest()
{
}

//------------------------------------------------------------------------------
bool ParticleTest::Init()
{
  int w = GRAPHICS.GetSwapChain(_swapChain)->_desc.BufferDesc.Width;
  int h = GRAPHICS.GetSwapChain(_swapChain)->_desc.BufferDesc.Height;

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
  _ctx->BeginFrame();

  int w = GRAPHICS.GetSwapChain(_swapChain)->_desc.BufferDesc.Width;
  int h = GRAPHICS.GetSwapChain(_swapChain)->_desc.BufferDesc.Height;

  _ctx->EndFrame();

  return true;
}

//------------------------------------------------------------------------------
bool ParticleTest::Close()
{
  return true;
}

//------------------------------------------------------------------------------
bool ParticleTest::Init(const char* config)
{
  return LoadProto(config, &_config);
}

//------------------------------------------------------------------------------
Effect* ParticleTest::Create(const char* name, GraphicsObjectHandle swapChain)
{
  return new ParticleTest(name, swapChain);
}

//------------------------------------------------------------------------------
const char* ParticleTest::Name()
{
  return "particle_test";
}
