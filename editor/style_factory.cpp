#include "style_factory.hpp"

using namespace editor;

namespace editor
{
  //----------------------------------------------------------------------------------
  void ApplyStyle(const Style* style, RectangleShape* shape)
  {
    shape->setFillColor(style->fillColor);
  }
}

//----------------------------------------------------------------------------------
StyledRectangle::StyledRectangle(const Style& style)
    : _style(style)
{
  UpdateStyle(style);
}

//----------------------------------------------------------------------------------
void StyledRectangle::UpdateStyle(const Style& style)
{
  _style = style;
  _shape.setFillColor(style.fillColor);
  _shape.setOutlineColor(style.outlineColor);
  _shape.setOutlineThickness(style.outlineThickness);
}

//----------------------------------------------------------------------------------
StyleFactory::~StyleFactory()
{
  _ownedShapes.clear();
}


//----------------------------------------------------------------------------------
bool StyleFactory::Init(const char* config)
{
  editor::protocol::Styles protoStyles;
  if (!LoadProto(config, &protoStyles))
  {
    return false;
  }

  editor::Styles styles = FromProtocol(protoStyles);
  for (const editor::Style& style : styles.styles)
  {
    _styles[style.id] = style;
  }

  // notify all owned shapes of changes
  for (auto& kv : _ownedShapes)
  {
    const string& id = kv.first;
    for (auto& shape : kv.second)
    {
      shape->UpdateStyle(_styles[id]);
    }

  }
  return true;
}


//----------------------------------------------------------------------------------
StyledRectangle* StyleFactory::CreateStyledRectangle(const string& id)
{
  if (const Style* style = GetStyle(id))
  {
    StyledRectangle* rect = new StyledRectangle(*style);
    _ownedShapes[id].push_back(unique_ptr<StyledShape>(rect));
    return rect;
  }
  return nullptr;
}

//----------------------------------------------------------------------------------
const editor::Style* StyleFactory::GetStyle(const string& id) const
{
  auto it = _styles.find(id);
  if (it == _styles.end())
    return nullptr;

  return &it->second;
}
