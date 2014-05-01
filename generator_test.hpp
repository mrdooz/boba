#pragma once

#include "effect.hpp"
#include "deferred_context.hpp"
#include "protocol/generator.pb.h"

namespace boba
{
  class DeferredContext;

  class GeneratorTest : public Effect
  {
  public:

    GeneratorTest(const string &name);
    ~GeneratorTest();
    virtual bool Init(const char* config) override;
    virtual bool Update(const UpdateState& state) override;
    virtual bool Render() override;
    virtual bool Close() override;

    virtual bool SaveSettings() override;

    static const char* Name();
    static Effect* Create(const char* name);

  private:

    virtual void WndProc(UINT message, WPARAM wParam, LPARAM lParam);

    string _configName;
    generator::Config _config;

    Matrix _oldWorldMatrix;
    Matrix _world;
    Matrix _view;
    Matrix _proj;

    u32 _numIndices;

    RenderObjects _meshObjects;

    bool _rotatingObject;
    Vector3 _v0;
    bool _dirtyFlag;

    Vector3 _cameraPos;
    Vector3 _cameraTarget;

    lua_State* _lua;
  };

}
