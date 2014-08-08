#pragma once

#include "protocol/effects_proto.hpp"
#include "effect_utils.hpp"

namespace editor
{
  void ApplyStyle(const Style* style, RectangleShape* shape);

  struct StyledShape
  {
    virtual ~StyledShape() {}
    virtual void UpdateStyle(const Style& style) = 0;
  };

  struct StyledRectangle : public StyledShape
  {
    StyledRectangle(const Style& style);
    virtual void UpdateStyle(const Style& style) override;

    RectangleShape _shape;
    Style _style;
  };

  struct StyleFactory
  {
    ~StyleFactory();
    bool Init(const char* config);
    StyledRectangle* CreateStyledRectangle(const string& id);
    const Style* GetStyle(const string& id) const;

    unordered_map<string, editor::Style> _styles;
    unordered_map<string, vector<unique_ptr<StyledShape>>> _ownedShapes;
  };

}