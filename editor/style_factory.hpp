#pragma once

#include "protocol/effects_proto.hpp"
#include "effect_utils.hpp"

namespace editor
{
  struct Style;

  void ApplyStyle(const Style* style, RectangleShape* shape);
  void ApplyStyle(const StyleSetting* style, RectangleShape* shape);

  struct Style
  {
    Style(const StyleSetting& s) : _style(s) {}
    void UpdateStyle(const StyleSetting& style);

    StyleSetting _style;
  };

  struct StyleFactory
  {
    ~StyleFactory();
    bool Init(const char* config);
    Style* CreateStyle(const string& id);
    const StyleSetting* GetStyle(const string& id) const;

    unordered_map<string, editor::StyleSetting> _styles;
    unordered_map<string, vector<unique_ptr<Style>>> _ownedStyles;
  };

  struct StyledRect
  {
    StyledRect() {}
    StyledRect(Style* style) : _style(style) {}
    void Apply() { ApplyStyle(_style, &_rect); }
    Style* _style;
    RectangleShape _rect;
  };

}