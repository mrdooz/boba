#pragma once

#include "protocol/effects_proto.hpp"
#include "style_factory.hpp"

namespace editor
{
  struct StyledRectangle;

  struct EffectRowEvent
  {
    enum class Type
    {
      VarSelected,
    };

    EffectRowEvent(Type type) : type(type) {}

    Type type;
  };

  //----------------------------------------------------------------------------------
  struct RowVar
  {
    RowVar(const Font& font, const string& name, FloatAnim* anim);

    struct VarFlagsF
    {
      enum Enum : u8 { Selected = 1 << 0, Editing = 1 << 1, Animating = 1 << 2, GraphMode  = 1 << 3, PreEdit = 1 << 4, };
      struct Bits { u8 selected : 1; u8 editing : 1; u8 animating : 1; u32 graphMode : 1; u32 preEdit : 1; };
    };

    typedef Flags<VarFlagsF> VarFlags;

    void Draw(RenderTexture& texture, bool drawKeyframes);
    void DrawKeyframes(RenderTexture& texture);

    bool OnMouseButtonPressed(const Event &event);
    bool OnMouseButtonReleased(const Event &event);
    bool OnMouseMoved(const Event &event);
    void OnEvent(RowVar* sender, const EffectRowEvent& event);
    void DrawGraph(RenderTexture& texture);
    Vector2f ToLocal(int x, int y) const;

    float PixelToValue(float value) const;
    float ValueToPixel(float value) const;
    FloatRect KeyframeRect(const FloatKey& k, float ofs);


    enum VisibleKeyframesFlags { AddBorderPoints = 1 << 0, AddOutside = 1 << 1 };

    void VisibleKeyframes(
        const Vector2f& size,
        u32 flags,
        vector<pair<Vector2f, const FloatKeyframe*>>* keyframes);

    Text _text;
    Font _font;
    FloatRect _bounds;

    string _name;
    float _value;
    FloatAnim* _anim;
    VarFlags _flags;
    StyledRect _keyframeRect;
    u32 _selectedKeyframe;
    FloatKeyframe _prevKeyframe;
    Vector2f _prevPos;

    float _minValue, _maxValue;
    float _realMinValue, _realMaxValue;
  };

  //----------------------------------------------------------------------------------
  struct EffectRow
  {
    struct RowFlagsF
    {
      enum Enum { Expanded = 1 << 0 };
      struct Bits { u32 expanded : 1; };
    };

    typedef Flags<RowFlagsF> RowFlags;

    EffectRow(const Font& font, const string& str, EffectRow* parent = nullptr);
    virtual ~EffectRow();

    void Draw(RenderTexture& texture, bool drawKeyframes);
    void Flatten(vector<EffectRow*>* res);
    void Reposition(float curY, float rowHeight);
    float RowHeight(float rowHeight);
    void DrawVars(RenderTexture& texture, bool drawKeyframes);

    virtual bool OnMouseMoved(const Event &event);
    virtual bool OnMouseButtonPressed(const Event &event);
    virtual bool OnMouseButtonReleased(const Event &event);
    virtual bool OnKeyReleased(const Event& event);

    virtual bool ToProtocol(google::protobuf::Message* msg) const { return true; }
    virtual bool FromProtocol(const google::protobuf::Message& msg) { return true; }
    u32 NumVars() const { return (u32)_vars.size(); }
    void SendEvent(RowVar* sender, const EffectRowEvent& event);

    string _str;
    RowFlags _flags;
    EffectRow* _parent;
    StyledRect _rect;
    vector<EffectRow*> _children;
    Text _text;
    Font _font;
    int _level;
    FloatRect _expandRect;
    FloatRect _varEditRect;
    StyledRect _keyframeRect;
    u32 _id;
    vector<RowVar*> _vars;
  };


  struct EffectRowTextPath;
  struct EffectRowNoise;
  //----------------------------------------------------------------------------------
  struct EffectRowPlexus : public EffectRow
  {
    EffectRowPlexus(
        const Font& font,
        const string& str,
        EffectRow* parent = nullptr);

    virtual bool ToProtocol(google::protobuf::Message* msg) const;
    virtual bool FromProtocol(const google::protobuf::Message& msg);
  };

  //----------------------------------------------------------------------------------
  struct EffectRowTextPath : public EffectRow
  {
    EffectRowTextPath(
        const Font& font,
        const string& str,
        EffectRow* parent = nullptr);

    virtual bool ToProtocol(google::protobuf::Message* msg) const;
    virtual bool FromProtocol(const google::protobuf::Message& msg);

    TextPath _textPath;
  };

  //----------------------------------------------------------------------------------
  struct EffectRowNoise : public EffectRow
  {
    EffectRowNoise(
        const Font& font,
        const string& str,
        EffectRow* parent = nullptr);

    virtual bool FromProtocol(const google::protobuf::Message& msg);
    virtual bool ToProtocol(google::protobuf::Message* msg) const;

    NoiseEffector _effector;
  };

}