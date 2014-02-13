#pragma once

#include "effect.hpp"
#include "protocol/particle.pb.h"

namespace boba
{
  class DeferredContext;

  class ParticleTest : public Effect
  {
  public:

    ParticleTest(const string &name, GraphicsObjectHandle swapChain);
    ~ParticleTest();
    virtual bool Init() override;
    virtual bool Update(const UpdateState& state) override;
    virtual bool Render() override;
    virtual bool Close() override;

    virtual bool Init(const char* config);

    static const char* Name();
    static Effect* Create(const char* name, GraphicsObjectHandle swapChain);

  private:
    particle::Config _config;
  };

}
