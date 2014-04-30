#pragma once

#include "effect.hpp"
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

    float Raycast(const Vector3& o, const Vector3& d);
    Vector3 ScreenToViewSpace(u32 x, u32 y);

    virtual void WndProc(UINT message, WPARAM wParam, LPARAM lParam);

    string _configName;
    generator::Config _config;
    GraphicsObjectHandle _vs;
    GraphicsObjectHandle _ps;
    GraphicsObjectHandle _layout;
    GraphicsObjectHandle _cbuffer;

    GraphicsObjectHandle _vb;
    GraphicsObjectHandle _ib;

    Matrix _oldWorldMatrix;
    Matrix _world;
    Matrix _view;
    Matrix _proj;

    u32 _numIndices;

    bool _rotatingObject;
    Vector3 _v0;
    bool _dirtyFlag;
  };

}
