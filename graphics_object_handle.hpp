#pragma once

namespace boba
{
  class GraphicsObjectHandle
  {
  public:
    enum Type
    {
      kInvalid = -1,    // NB: You have to compare _raw against kInvalid to test
      kContext,
      kVertexBuffer,
      kIndexBuffer,
      kConstantBuffer,
      kTexture,
      kResource,
      kRenderTarget,
      kShader,
      kInputLayout,
      kBlendState,
      kRasterizerState,
      kSamplerState,
      kDepthStencilState,
      kTechnique,
      kVertexShader,
      kGeometryShader,
      kPixelShader,
      kComputeShader,
      kFontFamily,
      kMaterial,
      kStructuredBuffer,
      kSwapChain,
      cNumTypes
    };

  private:
    friend class Graphics;
    enum
    { 
      cTypeBits = 8,
      cIdBits = 12,
      cDataBits = 32 - (cTypeBits + cIdBits),
    };

    static_assert(1 << GraphicsObjectHandle::cTypeBits > GraphicsObjectHandle::cNumTypes, "Not enough type bits");

    GraphicsObjectHandle(u32 type, u32 id) : _type(type), _id(id), _data(0) {}
    GraphicsObjectHandle(u32 type, u32 id, u32 data) : _type(type), _id(id), _data(data) 
    {
    }

    union
    {
      struct
      {
        u32 _type : cTypeBits;
        u32 _id : cIdBits;
        u32 _data : cDataBits;
      };
      u32 _raw;
    };
  public:
    GraphicsObjectHandle() : _raw(kInvalid) {}
    bool IsValid() const { return _raw != kInvalid; }
    u32 ToInt() const { return _raw; }
    u32 id() const { return _id; }
    u32 data() const { return _data; }
    Type type() const { return (Type)_type; }
  };

  static_assert(sizeof(GraphicsObjectHandle) <= sizeof(uint32_t), "GraphicsObjectHandle too large");
}
