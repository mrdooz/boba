#pragma once

#include "effect.hpp"
#include "protocol/particle.pb.h"

namespace boba
{
  class DeferredContext;

  class SceneTest : public Effect
  {
  public:

    SceneTest(const string &name);
    ~SceneTest();
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
