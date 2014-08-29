#pragma once

#include "protocol/effects_proto.hpp"
#include "style_factory.hpp"

namespace editor
{

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

    bool OnMouseButtonPressed(const Event& event);
    bool OnMouseButtonReleased(const Event& event);
    bool OnMouseMoved(const Event& event);
    bool OnKeyReleased(const Event& event);
    void OnEvent(RowVar* sender, const EffectRowEvent& event);
    void DrawGraph(RenderTexture& texture);
    Vector2f ToLocal(int x, int y) const;

    float PixelToValue(float value) const;
    float ValueToPixel(float value) const;
    FloatRect KeyframeRect(const FloatKey& k, float ofs);
    Vector2f KeyToPoint(const FloatKey& k);

    struct VisibleKeyframe
    {
      enum { FLAG_FIRST = 1 << 0, FLAG_LAST = 1 << 1, FLAG_VIRTUAL = 1 << 2, FLAG_SELECTED = 1 << 3};
      Vector2f p;
      const FloatKeyframe* keyframe;
      u32 flags;
    };

    void VisibleKeyframes(
        const Vector2f& size,
        bool addBorderPoints,
        bool addOutsidePoints,
        vector<VisibleKeyframe>* keyframes);

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
}
