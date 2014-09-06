#include "style_factory.hpp"
#include "protocol/editor_styles.pb.h"
#include "protocol/editor_proto.hpp"

using namespace editor;

namespace editor
{
  //----------------------------------------------------------------------------------
  void ApplyStyle(const Style* style, RectangleShape* shape)
  {
    ApplyStyle(&style->_style, shape);
  }

  //----------------------------------------------------------------------------------
  void ApplyStyle(const StyleSetting* style, RectangleShape* shape)
  {
    shape->setFillColor(style->fillColor);
    shape->setOutlineThickness(style->outlineThickness);
    shape->setOutlineColor(style->outlineColor);
  }
}

//----------------------------------------------------------------------------------
void editor::Style::UpdateStyle(const StyleSetting& style)
{
  _style = style;
}

//----------------------------------------------------------------------------------
StyleFactory::~StyleFactory()
{
  _ownedStyles.clear();
}


//----------------------------------------------------------------------------------
bool StyleFactory::Init(const char* config)
{
  protocol::editor::StyleSettings protoStyles;
  if (!LoadProto(config, &protoStyles))
  {
    return false;
  }

  editor::StyleSettings styles = FromProtocol(protoStyles);
  for (const editor::StyleSetting& style : styles.styleSetting)
  {
    _styles[style.id] = style;
  }

  // notify all owned shapes of changes
  for (auto& kv : _ownedStyles)
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
editor::Style* StyleFactory::CreateStyle(const string& id)
{
  if (const StyleSetting* style = GetStyle(id))
  {
    Style* s = new Style(*style);
    _ownedStyles[id].push_back(unique_ptr<Style>(s));
    return s;
  }
  return nullptr;
}

//----------------------------------------------------------------------------------
const StyleSetting* StyleFactory::GetStyle(const string& id) const
{
  auto it = _styles.find(id);
  if (it == _styles.end())
    return nullptr;

  return &it->second;
}
