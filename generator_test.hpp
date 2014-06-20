#pragma once

#include "effect.hpp"
#include "deferred_context.hpp"
#include "protocol/generator.pb.h"
#include "gpu_objects.hpp"

namespace boba
{
  class DeferredContext;

  struct PostProcess
  {
    PostProcess(DeferredContext* ctx);

    struct CBufferPS
    {
      Vector2 inputSize;
      Vector2 outputSize;
    };

    bool Init();
    void Setup();

    void Execute(
        const vector<GraphicsObjectHandle>& input,
        GraphicsObjectHandle output,
        GraphicsObjectHandle shader,
        const Color* clearColor,
        WCHAR* name);

    DeferredContext* _ctx;
    ConstantBuffer<CBufferPS> _cb;

    GraphicsObjectHandle _depthStencilState;
    GraphicsObjectHandle _blendState;
    GraphicsObjectHandle _rasterizerState;

    GraphicsObjectHandle _linearSamplerState;
    GraphicsObjectHandle _linearWrapSamplerState;
    GraphicsObjectHandle _linearBorderSamplerState;
    GraphicsObjectHandle _pointSamplerState;

    GraphicsObjectHandle _vsQuad;
  };

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
    void InitGeneratorScript();

    struct CBufferPerFrame
    {
      Matrix world;
      Matrix viewProj;
    };

    struct CBufferToneMapping
    {
      float tau;
      float key;
      float delta;
    };

    string _configName;
    generator::Spiky _spikyConfig;
    generator::Plane _planeConfig;

    Matrix _view;
    Matrix _invView;
    Matrix _proj;
    Matrix _prevRot;

    u32 _numIndices;

    GpuObjects _meshObjects;
    ConstantBuffer<CBufferPerFrame> _cb;

    bool _rotatingObject;
    Vector3 _v0;
    bool _dirtyFlag;
    struct RenderFlags { 
      enum Enum { Wireframe = 0x1, Luminance = 0x2, DebugDraw = 0x4, };
      struct Bits { u32 wireframe : 1; u32 luminance : 1; u32 debugDraw : 1; };
    };
    Flags<RenderFlags> _renderFlags;

    Vector3 _cameraPos, _cameraDir;
    unique_ptr<PostProcess> _postProcess;

    lua_State* _lua;

    GraphicsObjectHandle _renderTarget;
    GraphicsObjectHandle _psCopy;
    GraphicsObjectHandle _psLuminance;
    GraphicsObjectHandle _psAdaption;
    GraphicsObjectHandle _psComposite;

    // default states
    GraphicsObjectHandle _depthStencilState;
    GraphicsObjectHandle _blendState;
    GraphicsObjectHandle _rasterizerState;

    ConstantBuffer<CBufferToneMapping> _cbToneMapping;
    GraphicsObjectHandle _luminanceAdaption[2];
    u32 _curAdaption;
    UpdateState _updateState;
  };

}
