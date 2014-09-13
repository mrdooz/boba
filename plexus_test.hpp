#pragma once

#include "effect.hpp"
#include "deferred_context.hpp"

#pragma warning(push)
#pragma warning(disable: 4244 4267)
#include "protocol/generator.pb.h"
#pragma warning(pop)

#include "protocol/effect_plexus_types.hpp"

#include "gpu_objects.hpp"
#include "post_process.hpp"

namespace boba
{
  class DeferredContext;
  struct Mesh;
  struct Plexus;

  class PlexusTest : public Effect
  {
  public:

    PlexusTest(const string& name, u32 id);
    ~PlexusTest();
    virtual bool Init(const char* config) override;
    virtual bool Update(const UpdateState& state) override;
    virtual bool Render() override;
    virtual bool Close() override;

    virtual bool SaveSettings() override;
    virtual void ToProtocol(protocol::effect::EffectSetting* settings) const;
    virtual void FromProtocol(const std::string& str);

    static const char* Name();
    static Effect* Create(const char* name, u32 id);

  private:

    virtual void WndProc(UINT message, WPARAM wParam, LPARAM lParam);

    void RenderText();

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
    PostProcess* _postProcess;

    lua_State* _lua;

    ObjectHandle _renderTarget;
    ObjectHandle _psCopy;
    ObjectHandle _psLuminance;
    ObjectHandle _psAdaption;

    struct CBuffeComposite
    {
      float bloomMultiplier;
    };
    ConstantBuffer<CBuffeComposite> _cbComposite;

    ObjectHandle _psComposite;

    // default states
    GpuState _gpuState;

    struct CBufferToneMapping
    {
      float tau;
      float key;
      float delta;
      float ofs;
    };
    ConstantBuffer<CBufferToneMapping> _cbToneMapping;

    ObjectHandle _luminanceAdaption[2];
    u32 _curAdaption;
    UpdateState _updateState;

    struct CBufferBlur
    {
      Vector2 inputSize;
      float radius;
    };
    ConstantBuffer<CBufferBlur> _cbBlur;
    ObjectHandle _csBlurTranspose;
    ObjectHandle _csBlurX;
    ObjectHandle _csBlurY;
    ObjectHandle _csCopyTranspose;

    struct CBufferBloom
    {
      float threshold;
    };
    ConstantBuffer<CBufferBloom> _cbBloom;
    ObjectHandle _psThreshold;

    ObjectHandle _psEdgeDetect;

    PlexusConfig _plexusConfig;
    Plexus* _plexus;
  };

}
