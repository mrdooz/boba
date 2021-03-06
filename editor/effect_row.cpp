#include "effect_row.hpp"
#include "editor.hpp"
#include "row_var.hpp"
#include "timeline_window.hpp"
#include "property_window.hpp"
#include "protocol/effect_plexus_proto.hpp"

using namespace editor;
using namespace bristol;


//----------------------------------------------------------------------------------
EffectRow::EffectRow(const Font& font, const string& str, EffectRow* parent)
    : _str(str)
    , _parent(parent)
    , _font(font)
    , _level(0)
    , _id(0)
{
  _rect._style = STYLE_FACTORY.CreateStyle("default_row_color");
  _keyframeRect._style = STYLE_FACTORY.CreateStyle("keyframe_style");
  _keyframeRect._rect.setSize(Vector2f(6, 6));

  _text.setFont(font);
  _text.setCharacterSize(16);
  _flags.Set(EffectRow::RowFlagsF::Expanded);

  if (parent)
    _level = parent->_level + 1;
}

//----------------------------------------------------------------------------------
EffectRow::~EffectRow()
{
  for (EffectRow* e : _children)
    delete e;

  _children.clear();

  for (RowVar* r : _vars)
    delete r;

  _vars.clear();
}

//----------------------------------------------------------------------------------
void EffectRow::Flatten(vector<EffectRow*>* res)
{
  deque<EffectRow*> q({this});
  while (!q.empty())
  {
    EffectRow* cur = q.front();
    q.pop_front();

    res->push_back(cur);
    for (EffectRow* c : cur->_children)
      q.push_back(c);
  }
}

//----------------------------------------------------------------------------------
void EffectRow::Reposition(float curY, float rowHeight)
{
  // Set position and size of current and child rows and vars
  // This will depend on scroll position and row's expanded state

  const protocol::editor::Settings& settings = EDITOR.Settings();
  const Vector2f windowSize = TimelineWindow::_instance->GetSize();

  deque<EffectRow*> q({this});
  while (!q.empty())
  {
    EffectRow* cur = q.front();
    q.pop_front();

    cur->_rect._rect.setPosition(0, curY);
    cur->_rect._rect.setSize(Vector2f(windowSize.x, (1 + cur->NumVars()) * rowHeight));
    curY += rowHeight;

    // If row is expanded, add children
    if (cur->_flags.IsSet(EffectRow::RowFlagsF::Expanded))
    {
      for (EffectRow* c : cur->_children)
        q.push_back(c);
    }

    const RectangleShape& shape = cur->_rect._rect;

    cur->_expandRect = FloatRect(
        cur->_level * 15 + shape.getPosition().x,
        shape.getPosition().y,
        15,
        15);

    // Set bounds for the vars
    for (u32 i = 0; i < cur->_vars.size(); ++i)
    {
      RowVar* var = cur->_vars[i];
      float x = 40 + cur->_level * 15 + shape.getPosition().x;
      var->SetBounds(FloatRect(
          x,
          shape.getPosition().y + (i + 1) * rowHeight,
          settings.effect_view_width() - x,
          rowHeight));
    }
  }
}

//----------------------------------------------------------------------------------
float EffectRow::RowHeight(float rowHeight)
{
  float res = 0;

  deque<EffectRow*> q({this});
  while (!q.empty())
  {
    EffectRow* cur = q.front();
    q.pop_front();

    if (cur->_flags.IsSet(EffectRow::RowFlagsF::Expanded))
    {
      res += (1 + cur->NumVars()) * rowHeight;
      for (EffectRow* c : cur->_children)
        q.push_back(c);
    }
    else
    {
      res += rowHeight;
    }
  }

  return res;
}

//----------------------------------------------------------------------------------
bool EffectRow::OnMouseButtonPressed(const Event &event)
{
  const Vector2f& windowPos = TimelineWindow::_instance->GetPosition();
  int x = (int)(event.mouseButton.x - windowPos.x);
  int y = (int)(event.mouseButton.y - windowPos.y);
  Vector2f mousePos((float)x, (float)y);

  if (_expandRect.contains(mousePos))
  {
    _flags.Toggle(EffectRow::RowFlagsF::Expanded);
    return true;
  }

  // check the vars
  for (RowVar* v : _vars)
  {
    if (v->OnMouseButtonPressed(event))
      return true;
  }

  return false;
}

//----------------------------------------------------------------------------------
bool EffectRow::OnMouseMoved(const Event& event)
{
  if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
  {
    for (RowVar* v : _vars)
    {
      if (v->OnMouseMoved(event))
        return true;
    }
  }

  return false;
}

//----------------------------------------------------------------------------------
bool EffectRow::OnMouseButtonReleased(const Event &event)
{
  TimelineWindow* timeline = TimelineWindow::_instance;
  TimelineWindow::DisplayMode displayMode = timeline->GetDisplayMode();

  if (displayMode == TimelineWindow::DisplayMode::Graph)
  {
  }

  for (RowVar* v : _vars)
  {
    v->OnMouseButtonReleased(event);
  }

  return false;
}

//----------------------------------------------------------------------------------
bool EffectRow::OnKeyReleased(const Event& event)
{

  for (RowVar* var : _vars)
  {
    if (var->OnKeyReleased(event))
      return true;
  }

  return false;
}

//----------------------------------------------------------------------------------
void EffectRow::Draw(RenderTexture& texture, bool drawKeyframes)
{
  // Draw the keyframes before anything else so we can cheese a bit and draw over
  // any potential errant keyframes :)
  if (drawKeyframes)
  {
    for (RowVar* var : _vars)
      var->DrawKeyframes(texture);
  }

  const protocol::editor::Settings& settings = EDITOR.Settings();
  Color bgCol = common::FromProtocol(settings.effect_view_background_color());

  // draw background
  float w = (float)settings.effect_view_width();
  _rect._rect.setFillColor(bgCol);
  Vector2f size = _rect._rect.getSize();
  Vector2f windowSize = TimelineWindow::_instance->GetSize();
  _rect._rect.setSize(Vector2f(w, size.y));
  texture.draw(_rect._rect);

  // draw text
  _text.setString(_str);
  _text.setPosition(_rect._rect.getPosition() + Vector2f(20.f + _level * 15.f, 0.f) );
  _text.setColor(common::FromProtocol(settings.var_text_color()));
  texture.draw(_text);

  FloatRect bounds = _rect._rect.getGlobalBounds();
  DrawRow(texture, bounds.left, bounds.top, drawKeyframes ? windowSize.x : w, bounds.height,
    common::FromProtocol(settings.effect_line_color()));

  // draw expanded indicator
  VertexArray tri(sf::Triangles);
  float left = _level * 15 + _rect._rect.getPosition().x;
  float top = _rect._rect.getPosition().y;
  float y = top;

  if (!_flags.IsSet(EffectRow::RowFlagsF::Expanded))
  {
    const StyleSetting* s = STYLE_FACTORY.GetStyle("effect_icon_collapsed_color");
    // y increases downwards
    tri.append(Vertex(Vector2f(left+5, y+5), s->fillColor));
    tri.append(Vertex(Vector2f(left+5, y+15), s->fillColor));
    tri.append(Vertex(Vector2f(left+15, y+10), s->fillColor));
    texture.draw(tri);
  }
  else
  {
    // Row is expanded, so draw the Vars and children
    const StyleSetting* s = STYLE_FACTORY.GetStyle("effect_icon_expanded_color");
    tri.append(Vertex(Vector2f(left+5, y+5), s->fillColor));
    tri.append(Vertex(Vector2f(left+15, y+5), s->fillColor));
    tri.append(Vertex(Vector2f(left+10, y+15), s->fillColor));
    texture.draw(tri);

    DrawVars(texture, drawKeyframes);

    for (EffectRow* child : _children)
    {
      child->Draw(texture, drawKeyframes);
    }
  }
}

//----------------------------------------------------------------------------------
void EffectRow::DrawVars(RenderTexture& texture, bool drawKeyframes)
{
  for (RowVar* var : _vars)
  {
    var->Draw(texture, drawKeyframes);
  }
}

//----------------------------------------------------------------------------------
void EffectRow::SendEvent(RowVar* sender, const EffectRowEvent& event)
{
  for (RowVar* var : _vars)
  {
    var->OnEvent(sender, event);
  }

  for (EffectRow* e : _children)
    e->SendEvent(sender, event);
}


//----------------------------------------------------------------------------------
EffectRowPlexus::EffectRowPlexus(
    const Font& font,
    const string& str,
    EffectRow* parent)
    : EffectRow(font, str, parent)
{
}

//----------------------------------------------------------------------------------
bool EffectRowPlexus::ToProtocol(google::protobuf::Message* msg) const
{
  protocol::effect::EffectSetting* proto = static_cast<protocol::effect::EffectSetting*>(msg);
  assert(!_parent);
  proto->set_type(protocol::effect::EffectSetting_Type_Plexus);

  protocol::effect::plexus::PlexusConfig* plexus = proto->mutable_plexus_config();
  for (const EffectRow* row : _children)
  {
    row->ToProtocol(plexus);
  }

  return true;
}

//----------------------------------------------------------------------------------
bool EffectRowPlexus::FromProtocol(const google::protobuf::Message& proto)
{
  const protocol::effect::plexus::PlexusConfig& p =
      static_cast<const protocol::effect::plexus::PlexusConfig&>(proto);

  for (const protocol::effect::plexus::TextPathConfig& textPath : p.text_paths())
  {
    string str = to_string("TextPath: %s", textPath.text().c_str());

    _children.push_back(new EffectRowTextPath(_font, str, this));
    _children.back()->FromProtocol(textPath);
  }

  for (const protocol::effect::plexus::NoiseEffectorConfig& effector : p.noise_effectors())
  {
    string str = to_string("Noise (%s)",
      effector.apply_to() == protocol::effect::plexus::NoiseEffectorConfig_ApplyTo_Position
          ? "POS" : "SCALE");

    _children.push_back(new EffectRowNoise(_font, str, this));
    _children.back()->FromProtocol(effector);
  }

  return true;
}

//----------------------------------------------------------------------------------
EffectRowTextPath::EffectRowTextPath(
    const Font& font,
    const string& str,
    EffectRow* parent)
  : EffectRow(font, str, parent)
{
}

//----------------------------------------------------------------------------------
bool EffectRowTextPath::ToProtocol(google::protobuf::Message* msg) const
{
  protocol::effect::plexus::PlexusConfig* proto = static_cast<protocol::effect::plexus::PlexusConfig*>(msg);
  proto->add_text_paths()->set_text(_str);
  return true;
}

//----------------------------------------------------------------------------------
bool EffectRowTextPath::FromProtocol(const google::protobuf::Message& proto)
{
  const protocol::effect::plexus::TextPathConfig& p 
    = static_cast<const protocol::effect::plexus::TextPathConfig&>(proto);
  _textPath = effect::plexus::FromProtocol(p);
  return true;
}

//----------------------------------------------------------------------------------
EffectRowNoise::EffectRowNoise(
    const Font& font,
    const string& str,
    EffectRow* parent)
    : EffectRow(font, str, parent)
{
  _vars.push_back(new RowVar(_font, "x", &_effector.displacement.x));
  _vars.push_back(new RowVar(_font, "y", &_effector.displacement.y));
  _vars.push_back(new RowVar(_font, "z", &_effector.displacement.z));
}

//----------------------------------------------------------------------------------
bool EffectRowNoise::FromProtocol(const google::protobuf::Message& proto)
{
  const protocol::effect::plexus::NoiseEffectorConfig& p 
    = static_cast<const protocol::effect::plexus::NoiseEffectorConfig&>(proto);
  _effector = effect::plexus::FromProtocol(p);
  return true;
}

//----------------------------------------------------------------------------------
bool EffectRowNoise::ToProtocol(google::protobuf::Message* msg) const
{
  protocol::effect::plexus::PlexusConfig* proto 
    = static_cast<protocol::effect::plexus::PlexusConfig*>(msg);

  effect::plexus::ToProtocol(_effector, proto->add_noise_effectors());
  return true;
}
