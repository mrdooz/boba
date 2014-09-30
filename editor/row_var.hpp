#pragma once

#include "protocol/common_types.hpp"
#include "protocol/effect_plexus_types.hpp"
#include "style_factory.hpp"

namespace editor
{

  struct EffectRowEvent
  {
    enum class Type
    {
      VarSelected,
      KeyframeSelected,
      KeyframeUpdated,
    };

    EffectRowEvent(Type type) : type(type), data(nullptr) {}
    EffectRowEvent(Type type, void* data) : type(type), data(data) {}

    Type type;
    void* data;
  };

  struct TextStyle
  {
    struct StyleFlagsF
    {
      enum Enum : u8 { HasStyle = 1 << 0, HasColor = 1 << 1 };
      struct Bits { u8 hasStyle : 1; u8 hasColor : 1; };
    };

    typedef Flags<StyleFlagsF> StyleFlags;


    TextStyle& Style(u32 style)
    {
      _flags.Set(StyleFlagsF::HasStyle);
      _style = style;
      return *this;
    }

    TextStyle& Color(const sf::Color& color)
    {
      _flags.Set(StyleFlagsF::HasColor);
      _color = color;
      return *this;
    }

    void Apply(Text& text)
    {
      if (_flags.IsSet(StyleFlagsF::HasStyle)) text.setStyle(_style);
      if (_flags.IsSet(StyleFlagsF::HasColor)) text.setColor(_color);
    }

    StyleFlags _flags;
    u32 _style;
    sf::Color _color;
  };

  struct TextStylePredicate
  {
    typedef function<bool()> fnPredicate;

    TextStylePredicate& Default(const TextStyle& defaultStyle)
    {
      _defaultStyle = defaultStyle;
      return *this;
    }

    TextStylePredicate& Predicate(const fnPredicate& pred, const TextStyle& style)
    {
      _styles.push_back(make_pair(pred, style));
      return *this;
    }

    void ApplyAndDraw(RenderTexture& texture)
    {
      // check if any of the predicates match
      for (auto& p : _styles)
      {
        if (p.first())
        {
          p.second.Apply(_text);
          texture.draw(_text);
          return;
        }
      }

      // nothing matched, so apply default style
      _defaultStyle.Apply(_text);
      texture.draw(_text);
    }

    vector<pair<fnPredicate, TextStyle>> _styles;
    TextStyle _defaultStyle;

    Text _text;
  };


  //----------------------------------------------------------------------------------
  struct RowVar
  {
    RowVar(const Font& font, const string& name, common::FloatAnim* anim);

    struct VarFlagsF
    {
      enum Enum : u8 { Selected = 1 << 0, Editing = 1 << 1, Animating = 1 << 2, GraphMode  = 1 << 3, PreEdit = 1 << 4, Moving = 1 << 5};
      struct Bits { u8 selected : 1; u8 editing : 1; u8 animating : 1; u32 graphMode : 1; u32 preEdit : 1; u32 moving : 1; };
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
    FloatRect KeyframeRect(const common::FloatKey& k, float ofs);
    Vector2f KeyToPoint(const common::FloatKey& k);

    struct VisibleKeyframe
    {
      enum { FLAG_FIRST = 1 << 0, FLAG_LAST = 1 << 1, FLAG_VIRTUAL = 1 << 2, FLAG_SELECTED = 1 << 3};
      Vector2f p;
      const common::FloatKeyframe* keyframe;
      u32 flags;
    };

    void VisibleKeyframes(
        const Vector2f& size,
        bool addBorderPoints,
        bool addOutsidePoints,
        vector<VisibleKeyframe>* keyframes);

    void SetBounds(const FloatRect& bounds);

    Text _text;
    Font _font;
    RectangleShape _bgRect;
    FloatRect _bounds;

    string _name;
    // value is used when changing the keyframe value by dragging on the text field
    float _value;
    common::FloatAnim* _anim;
    VarFlags _flags;
    StyledRect _keyframeRect;
    u32 _selectedKeyframe;
    common::FloatKeyframe _prevKeyframe;
    Vector2f _prevPos;

    TextStylePredicate _animationText;

    float _minValue, _maxValue, _step;
  };
}
