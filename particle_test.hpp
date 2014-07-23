#pragma once

#include "effect.hpp"

#pragma warning(push)
#pragma warning(disable: 4244 4267)
#include "protocol/particle.pb.h"
#pragma warning(pop)

namespace boba
{
  class DeferredContext;

  class ParticleTest : public Effect
  {
  public:

    ParticleTest(const string &name);
    ~ParticleTest();
    virtual bool Show();
    virtual bool Hide();
    virtual bool Init(const char* config) override;
    virtual bool Update(const UpdateState& state) override;
    virtual bool Render() override;
    virtual bool Close() override;

    virtual bool SaveSettings() override;

    static const char* Name();
    static Effect* Create(const char* name);

  private:
    string _configName;
    particle::Config _config;
    GraphicsObjectHandle _texture;
    GraphicsObjectHandle _vs;
    GraphicsObjectHandle _ps;
    GraphicsObjectHandle _samplerState;
    GraphicsObjectHandle _cbuffer;
  };

}
