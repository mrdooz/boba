#pragma once

#include "effect.hpp"

#pragma warning(push)
#pragma warning(disable: 4244 4267)
#include "protocol/effect_settings_particle.pb.h"
#include "protocol/effect_particle_proto.hpp"
#pragma warning(pop)

#include "protocol/effect_particle_types.hpp"

namespace boba
{
  class DeferredContext;

  class SceneTest : public Effect
  {
  public:

    SceneTest(const string& name, u32 id);
    ~SceneTest();
    virtual bool Init(const protocol::effect::EffectSetting& config) override;
    virtual bool Update(const UpdateState& state) override;
    virtual bool Render() override;
    virtual bool Close() override;

    virtual bool SaveSettings() override;

    static const char* Name();
    static Effect* Create(const char* name, u32 id);

  private:

    float Raycast(const Vector3& o, const Vector3& d);
    Vector3 ScreenToViewSpace(u32 x, u32 y);

    virtual void WndProc(UINT message, WPARAM wParam, LPARAM lParam);

    effect::particle::ParticleConfig _config;
    ObjectHandle _texture;
    ObjectHandle _vs;
    ObjectHandle _ps;
    ObjectHandle _samplerState;
    ObjectHandle _cbuffer;

    Matrix _view;
    Matrix _proj;

    bool _rotatingObject;
    Vector3 _v0;
    Matrix _oldWorldMatrix;
  };

}
