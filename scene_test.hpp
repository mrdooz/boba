#pragma once

#include "effect.hpp"

#pragma warning(push)
#pragma warning(disable: 4244 4267)
#include "protocol/particle.pb.h"
#pragma warning(pop)


namespace boba
{
  class DeferredContext;

  class SceneTest : public Effect
  {
  public:

    SceneTest(const string& name, u32 id);
    ~SceneTest();
    virtual bool Init(const char* config) override;
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

    string _configName;
    particle::Config _config;
    GraphicsObjectHandle _texture;
    GraphicsObjectHandle _vs;
    GraphicsObjectHandle _ps;
    GraphicsObjectHandle _samplerState;
    GraphicsObjectHandle _cbuffer;

    Matrix _view;
    Matrix _proj;

    bool _rotatingObject;
    Vector3 _v0;
    Matrix _oldWorldMatrix;
  };

}
