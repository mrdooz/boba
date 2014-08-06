#include "style_factory.hpp"

using namespace editor;

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
