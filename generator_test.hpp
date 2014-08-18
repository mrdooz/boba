#pragma once

#include "effect.hpp"
#include "deferred_context.hpp"

#pragma warning(push)
#pragma warning(disable: 4244 4267)
#include "protocol/generator.pb.h"
#pragma warning(pop)

#include "gpu_objects.hpp"
#include "post_process.hpp"

namespace boba
{
  class DeferredContext;
  struct Mesh;


  class GeneratorTest : public Effect
  {
  public:

    GeneratorTest(const string& name, u32 id);
    ~GeneratorTest();
    virtual bool Init(const char* config) override;
    virtual bool Update(const UpdateState& state) override;
    virtual bool Render() override;
    virtual bool Close() override;

    virtual bool SaveSettings() override;
    virtual void ToProtocol(effect::protocol::EffectSetting* settings) const;
    virtual void FromProtocol(const std::string& str);

    static const char* Name();
    static Effect* Create(const char* name, u32 id);

  private:

    virtual void WndProc(UINT message, WPARAM wParam, LPARAM lParam);

    void RenderText();
    void RenderSpiky();
    void RenderPlane();
    void InitGeneratorScript();

    string _configName;
    generator::Spiky _spikyConfig;
    generator::Plane _planeConfig;

    Matrix _view;
    Matrix _invView;
    Matrix _proj;
    Matrix _prevRot;

    u32 _numIndices;

    GpuObjects _meshObjects;
    struct CBufferPerFrame
    {
      Matrix world;
      Matrix viewProj;
    };
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

    struct CBuffeComposite
    {
      float bloomMultiplier;
    };
    ConstantBuffer<CBuffeComposite> _cbComposite;

    GraphicsObjectHandle _psComposite;

    // default states
    GraphicsObjectHandle _depthStencilState;
    GraphicsObjectHandle _blendState;
    GraphicsObjectHandle _rasterizerState;

    struct CBufferToneMapping
    {
      float tau;
      float key;
      float delta;
      float ofs;
    };
    ConstantBuffer<CBufferToneMapping> _cbToneMapping;

    GraphicsObjectHandle _luminanceAdaption[2];
    u32 _curAdaption;
    UpdateState _updateState;

    struct CBufferBlur
    {
      Vector2 inputSize;
      float radius;
    };
    ConstantBuffer<CBufferBlur> _cbBlur;
    GraphicsObjectHandle _csBlurTranspose;
    GraphicsObjectHandle _csBlurX;
    GraphicsObjectHandle _csBlurY;
    GraphicsObjectHandle _csCopyTranspose;

    struct CBufferBloom
    {
      float threshold;
    };
    ConstantBuffer<CBufferBloom> _cbBloom;
    GraphicsObjectHandle _psThreshold;

    GraphicsObjectHandle _psEdgeDetect;
  };

}
