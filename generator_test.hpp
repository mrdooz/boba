#pragma once

#include "effect.hpp"
#include "deferred_context.hpp"
#include "protocol/generator.pb.h"
#include "gpu_objects.hpp"

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

    void RenderSpiky();
    void RenderPlane();

    string _configName;
    generator::Spiky _spikyConfig;
    generator::Plane _planeConfig;

    Matrix _view;
    Matrix _invView;
    Matrix _proj;
    Matrix _prevRot;

    u32 _numIndices;

    GpuObjects _meshObjects;

    bool _rotatingObject;
    Vector3 _v0;
    bool _dirtyFlag;
    bool _debugDraw;
    bool _wireframe;

    Vector3 _cameraPos, _cameraDir;

    lua_State* _lua;
  };

}
