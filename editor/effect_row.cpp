#include "effect_row.hpp"
#include "editor_windows.hpp"
#include "editor.hpp"
#import "flags.hpp"

using namespace editor;
using namespace bristol;

#pragma warning(disable: 4244)

//----------------------------------------------------------------------------------
RowVar::RowVar(
    const Font& font,
    const string& name,
    FloatAnim* anim)
    : _font(font)
    , _name(name)
    , _anim(anim)
{
  _text.setFont(font);
  _text.setCharacterSize(16);
}

//----------------------------------------------------------------------------------
void RowVar::DrawKeyframes(RenderTexture& texture)
{
  const editor::protocol::Settings& settings = EDITOR.Settings();
  const Vector2f& windowSize = TimelineWindow::_instance->GetSize();
  float h = settings.effect_row_height();

  // draw the keyframes
  VertexArray curLine(sf::Lines);
  int x = settings.effect_view_width();
  int w = windowSize.x - x;
  int y = _bounds.top + h / 0.5f;

  curLine.append(sf::Vertex(Vector2f(x, y), Color::White));
  curLine.append(sf::Vertex(Vector2f(x + w, y), Color::White));
  texture.draw(curLine);
}

//----------------------------------------------------------------------------------
void RowVar::Draw(RenderTexture& texture, const Vector2f& pos)
{
  // TODO: draw animated icon

  u32 now = EDITOR.CurTime().total_milliseconds();
  float v = Interpolate<float>(*_anim, now);

  const Style* style = _flags.IsSet(VarFlagsF::Editing)
      ? STYLE_FACTORY.GetStyle("var_text_editing")
      : STYLE_FACTORY.GetStyle("var_text_normal");

  _text.setString(to_string("%s: %.2f", _name.c_str(), v).c_str());
  _text.setColor(style->fillColor);
  _text.setStyle(style->fontStyle);

  _text.setPosition(pos);
  texture.draw(_text);
}

//----------------------------------------------------------------------------------
EffectRow::EffectRow(
    const Font& font,
    const string& str,
    EffectRow* parent)
    : _str(str)
    , _parent(parent)
    , _font(font)
    , _level(0)
    , _id(0)
{
  _rect = STYLE_FACTORY.CreateStyledRectangle("default_row_color");
  _keyframeRect = STYLE_FACTORY.CreateStyledRectangle("keyframe_style");
  _keyframeRect->_shape.setSize(Vector2f(6, 6));

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

  const editor::protocol::Settings& settings = EDITOR.Settings();
  const Vector2f windowSize = TimelineWindow::_instance->GetSize();

  deque<EffectRow*> q({this});
  while (!q.empty())
  {
    EffectRow* cur = q.front();
    q.pop_front();

    cur->_rect->_shape.setPosition(0, curY);
    cur->_rect->_shape.setSize(Vector2f(windowSize.x, (1 + cur->NumVars()) * rowHeight));
    curY += rowHeight;
    if (cur->_flags.IsSet(EffectRow::RowFlagsF::Expanded))
    {
      for (EffectRow* c : cur->_children)
        q.push_back(c);
    }

    const RectangleShape& shape = cur->_rect->_shape;

    cur->_expandRect = FloatRect(
        cur->_level * 15 + shape.getPosition().x,
        shape.getPosition().y,
        15,
        15);

    // Set bounds for the vars
    for (u32 i = 0; i < cur->_vars.size(); ++i)
    {
      RowVar& var = cur->_vars[i];
      float x = cur->_level * 15 + shape.getPosition().x;
      var._bounds = FloatRect(
          x,
          shape.getPosition().y + (i + 1) * rowHeight,
          (settings.effect_view_width() - x) / 2,
          rowHeight);
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
void EffectRow::Draw(RenderTexture& texture, bool drawKeyframes)
{
  const editor::protocol::Settings& settings = EDITOR.Settings();
  Color rowCol = ::FromProtocol(_flags.IsSet(EffectRow::RowFlagsF::Selected)
      ? settings.selected_row_color()
      : settings.default_row_color());

  // draw background
  _rect->_shape.setFillColor(rowCol);
  Vector2f size = _rect->_shape.getSize();
  Vector2f windowSize = TimelineWindow::_instance->GetSize();
  _rect->_shape.setSize(Vector2f(drawKeyframes ? windowSize.x : settings.effect_view_width(), size.y));
  texture.draw(_rect->_shape);

  // fill the background if in graph mode
  if (!drawKeyframes)
  {
    RectangleShape rect;
    rect.setPosition(settings.effect_view_width(), settings.ticker_height());
    rect.setSize(Vector2f(
        windowSize.x - settings.effect_view_width(),
        windowSize.y - settings.ticker_height()));

    rect.setFillColor(STYLE_FACTORY.GetStyle("default_row_color")->fillColor);
    texture.draw(rect);
  }

  // draw text
  _text.setString(_str);
  _text.setPosition(_rect->_shape.getPosition() + Vector2f(20 + _level * 15, 0) );
  texture.draw(_text);

  // draw expanded indicator
  VertexArray tri(sf::Triangles);
  float left = _level * 15 + _rect->_shape.getPosition().x;
  float top = _rect->_shape.getPosition().y;
  float y = top;

  if (!_flags.IsSet(EffectRow::RowFlagsF::Expanded))
  {
    const Style* s = STYLE_FACTORY.GetStyle("effect_icon_collapsed_color");
    // y increases downwards
    tri.append(sf::Vertex(Vector2f(left+5, y+5), s->fillColor));
    tri.append(sf::Vertex(Vector2f(left+5, y+15), s->fillColor));
    tri.append(sf::Vertex(Vector2f(left+15, y+10), s->fillColor));
    texture.draw(tri);
  }
  else
  {
    // Row is expanded, so draw the Vars and children
    const Style* s = STYLE_FACTORY.GetStyle("effect_icon_expanded_color");
    tri.append(sf::Vertex(Vector2f(left+5, y+5), s->fillColor));
    tri.append(sf::Vertex(Vector2f(left+15, y+5), s->fillColor));
    tri.append(sf::Vertex(Vector2f(left+10, y+15), s->fillColor));
    texture.draw(tri);

    DrawVars(texture, drawKeyframes);

    for (EffectRow* child : _children)
    {
      child->Draw(texture, drawKeyframes);
    }
  }
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
  effect::protocol::EffectSetting* proto = static_cast<effect::protocol::EffectSetting*>(msg);
  assert(!_parent);
  proto->set_type(effect::protocol::EffectSetting_Type_Plexus);

  effect::protocol::plexus::Plexus plexus;
  for (const EffectRow* row : _children)
  {
    row->ToProtocol(&plexus);
  }

  proto->set_config_msg(plexus.SerializeAsString());

  return true;
}

//----------------------------------------------------------------------------------
bool EffectRowPlexus::FromProtocol(const google::protobuf::Message& proto)
{
  const effect::protocol::plexus::Plexus& p = static_cast<const effect::protocol::plexus::Plexus&>(proto);

  for (const effect::protocol::plexus::TextPath& textPath : p.text_paths())
  {
    string str = to_string("TextPath: %s", textPath.text().c_str());

    _children.push_back(new EffectRowTextPath(_font, str, this));
    _children.back()->FromProtocol(textPath);
  }

  for (const effect::protocol::plexus::NoiseEffector& effector : p.noise_effectors())
  {
    string str = to_string("Noise (%s)",
      effector.apply_to() == effect::protocol::plexus::NoiseEffector_ApplyTo_Position ? "POS" : "SCALE");

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
  effect::protocol::plexus::Plexus* proto = static_cast<effect::protocol::plexus::Plexus*>(msg);
  proto->add_text_paths()->set_text(_str);
  return true;
}

//----------------------------------------------------------------------------------
bool EffectRowTextPath::FromProtocol(const google::protobuf::Message& proto)
{
  const effect::protocol::plexus::TextPath& p = static_cast<const effect::protocol::plexus::TextPath&>(proto);
  _textPath = ::FromProtocol(p);
  return true;
}

//----------------------------------------------------------------------------------
EffectRowNoise::EffectRowNoise(
    const Font& font,
    const string& str,
    EffectRow* parent)
    : EffectRow(font, str, parent)
    , _editingIdx(-1)
    , _selectedKeyframe(nullptr)
    , _graphMode(0)
{
  _vars.push_back(RowVar(_font, "x", &_effector.displacement.x));
  _vars.push_back(RowVar(_font, "y", &_effector.displacement.y));
  _vars.push_back(RowVar(_font, "z", &_effector.displacement.z));
}

//----------------------------------------------------------------------------------
void EffectRowNoise::DrawVars(RenderTexture& texture, bool drawKeyframes)
{
  Vector2f size = TimelineWindow::_instance->GetSize();
  const editor::protocol::Settings& settings = EDITOR.Settings();
  float h = settings.effect_row_height();

  for (u32 i = 0; i < _vars.size(); ++i)
  {
    RowVar& var = _vars[i];
    var.Draw(texture, _rect->_shape.getPosition() + Vector2f(20 + _level * 15, h*(i+1)));
  }

  if (drawKeyframes)
  {
    for (u32 i = 0; i < _vars.size(); ++i)
    {
      RowVar& var = _vars[i];
      var.DrawKeyframes(texture);
    }
  }


//  u32 now = EDITOR.CurTime().total_milliseconds();
//  float vx = Interpolate<float>(_effector.displacement.x, now);
//  float vy = Interpolate<float>(_effector.displacement.y, now);
//  float vz = Interpolate<float>(_effector.displacement.z, now);
////  Vector3f v = Interpolate<Vector3f>(_effector.displacement, EDITOR.CurTime().total_milliseconds());
//
//  const Style* editingStyle = STYLE_FACTORY.GetStyle("var_text_editing");
//  const Style* normalStyle = STYLE_FACTORY.GetStyle("var_text_normal");
//
//  static const char* suffix[] = {
//      "x: ",
//      "y: ",
//      "z: "
//  };
//
//  const auto& fnStyle = [this, editingStyle, normalStyle](float v, const char* prefix){
//      if (curIdx == _editingIdx)
//      {
//        _text.setString(to_string("%s%s", prefix, _curEdit.c_str()).c_str());
//        _text.setColor(editingStyle->fillColor);
//        _text.setStyle(editingStyle->fontStyle);
//      }
//      else
//      {
//        _text.setString(to_string("%s%.2f", prefix,  v).c_str());
//        _text.setColor(normalStyle->fillColor);
//        _text.setStyle(normalStyle->fontStyle);
//      }
//  };
//
//  // draw the vars
//  for (u32 i = 0; i < 3; ++i)
//  {
//    fnStyle(i);
//    _text.setPosition(_rect->_shape.getPosition() + Vector2f(20 + _level * 15, h*(i+1)));
//    texture.draw(_text);
//  }
//
//  if (drawKeyframes)
//  {
//    DrawKeyframes(texture, size);
//  }
//
//  // Reset style
//  fnStyle(-2);
}

//----------------------------------------------------------------------------------
void EffectRowNoise::DrawKeyframes(RenderTexture& texture, const Vector2f& size)
{
  const editor::protocol::Settings& settings = EDITOR.Settings();
  float h = settings.effect_row_height();

  // draw the keyframes
  VertexArray curLine(sf::Lines);
  int w = size.x - settings.effect_view_width();
  int x = settings.effect_view_width();
  float shapeY = _rect->_shape.getPosition().y;

  for (int i = 0; i < 3; ++i)
  {
    int y = shapeY + h * (i+1.5f);
    curLine.append(sf::Vertex(Vector2f(x, y), Color::White));
    curLine.append(sf::Vertex(Vector2f(x + w, y), Color::White));
  }
  texture.draw(curLine);

//  for (u32 i = 0; i < _effector.displacement.keyframe.size(); ++i)
//  {
//    const Vector3Keyframe& keyframe = _effector.displacement.keyframe[i];
//
//    ApplyStyle(STYLE_FACTORY.GetStyle(&keyframe == _selectedKeyframe ?
//        "keyframe_style_selected" : "keyframe_style"), &_keyframeRect->_shape);
//
//    int x = TimelineWindow::_instance->TimeToPixel(milliseconds(keyframe.time));
//    if (x < size.x)
//    {
//      // draw for each var
//      for (u32 j = 0; j < 3; ++j)
//      {
//        int y = shapeY + h * (j+1.5f) - 3;
//        _keyframeRect->_shape.setPosition(x - 3, y);
//        texture.draw(_keyframeRect->_shape);
//      }
//    }
//  }
}

//----------------------------------------------------------------------------------
Vector3f EffectRowNoise::PixelToValue(int x) const
{
  // x = bottom - h * (value - minValue) / span
  // x - bottom = -h..
  // ==> value = (bottom - x) * span / h + minValue

  Vector2f size = TimelineWindow::_instance->GetSize();
  const editor::protocol::Settings& settings = EDITOR.Settings();
  float topY = settings.ticker_height();
  float h = size.y - topY;
  float bottom = size.y - 1;

  Vector3f span = _maxValue - _minValue;

  return (bottom - x) * span / h + _minValue;
}

//----------------------------------------------------------------------------------
int EffectRowNoise::ValueToPixel(const Vector3f& value)
{
  // x = bottom - h * (value - minValue) / span

  Vector2f size = TimelineWindow::_instance->GetSize();
  const editor::protocol::Settings& settings = EDITOR.Settings();
  float topY = settings.ticker_height();
  float h = size.y - topY;
  float bottom = size.y - 1;

  float span = ExtractGraphValue(_maxValue - _minValue);
  float v = ExtractGraphValue(value - _minValue);

  return (int)bottom - h * v / span;
}

//----------------------------------------------------------------------------------
float EffectRowNoise::ExtractGraphValue(const Vector3f& value) const
{
  switch (_graphMode)
  {
    case 1: return value.x;
    case 2: return value.y;
    case 3: return value.z;
    default: assert(!"oh noes!"); return 0;
  }
}

//----------------------------------------------------------------------------------
float& EffectRowNoise::ExtractGraphValue(Vector3f& value)
{
  switch (_graphMode)
  {
    case 1: return value.x;
    case 2: return value.y;
    case 3: return value.z;
  }
  assert(!"fix me!");
  return value.x;
}

//----------------------------------------------------------------------------------
float EffectRowNoise::CalcGraphValue(const Vector3f& value) const
{
  Vector2f size = TimelineWindow::_instance->GetSize();
  const editor::protocol::Settings& settings = EDITOR.Settings();
  float topY = settings.ticker_height();
  float h = size.y - topY;
  float bottom = size.y - 1;

  Vector3f span = _maxValue - _minValue;

  // scale the value so min/max covers the entire height
  Vector3f t = value - _minValue;
  switch (_graphMode)
  {
    // x = bottom - h * (value - minValue) / span
    case 1: return bottom - h * t.x / span.x;
    case 2: return bottom - h * t.y / span.y;
    case 3: return bottom - h * t.z / span.z;
    default: return 0.f;
  }
}

//----------------------------------------------------------------------------------
void EffectRowNoise::VisibleKeyframes(
    const Vector2f& size,
    bool addBorderPoints,
    vector<pair<Vector2f, Vector3Keyframe*>>* keyframes)
{
  #if 0
  const editor::protocol::Settings& settings = EDITOR.Settings();
  float ofs = settings.effect_view_width();

  TimelineWindow* timeline = TimelineWindow::_instance;

  // find min/max values for keyframes within the view
  time_duration minTime = timeline->PixelToTime(0);
  time_duration maxTime = timeline->PixelToTime(size.x);

  Vector3f value0 = Interpolate<Vector3f>(_effector.displacement, minTime);
  Vector3f valueLast = Interpolate<Vector3f>(_effector.displacement, maxTime);

  vector<Vector3Keyframe*> validKeyframes;

  if (!_selectedKeyframe)
  {
    _minValue = Min(
        Interpolate<Vector3f>(_effector.displacement, minTime),
        Interpolate<Vector3f>(_effector.displacement, maxTime));

    _maxValue = Max(
        Interpolate<Vector3f>(_effector.displacement, minTime),
        Interpolate<Vector3f>(_effector.displacement, maxTime));


    for (Vector3Keyframe& keyframe : _effector.displacement.keyframe)
    {
      if (keyframe.time < minTime.total_milliseconds())
        continue;

      if (keyframe.time > maxTime.total_milliseconds())
        break;

      validKeyframes.push_back(&keyframe);

      _minValue = Min(_minValue, keyframe.value);
      _maxValue = Max(_maxValue, keyframe.value);
    }

    _realMinValue = _minValue;
    _realMaxValue = _maxValue;

    Vector3f step;
    CalcCeilAndStep(_maxValue, &step, &_maxValue);
    _minValue = _realMinValue - step;
  }
  else
  {
    for (Vector3Keyframe& keyframe : _effector.displacement.keyframe)
    {
      if (keyframe.time < minTime.total_milliseconds())
        continue;

      if (keyframe.time > maxTime.total_milliseconds())
        break;

      validKeyframes.push_back(&keyframe);
    }
  }

  if (addBorderPoints)
  {
    keyframes->push_back(make_pair(Vector2f(ofs, CalcGraphValue(value0)), nullptr));
  }

  for (Vector3Keyframe* keyframe : validKeyframes)
  {
    Vector2f p = Vector2f(
        timeline->TimeToPixel(milliseconds(keyframe->time)),
        CalcGraphValue(keyframe->value));

    keyframes->push_back(make_pair(p, keyframe));
  }

  if (addBorderPoints)
  {
    keyframes->push_back(make_pair(Vector2f(size.x, CalcGraphValue(valueLast)), nullptr));
  }
#endif
}

//----------------------------------------------------------------------------------
void EffectRowNoise::CalcCeilAndStep(
    const Vector3f& value,
    Vector3f* stepValue,
    Vector3f* ceilValue)
{
  CalcCeilAndStep(value.x, &stepValue->x, &ceilValue->x);
  CalcCeilAndStep(value.y, &stepValue->y, &ceilValue->y);
  CalcCeilAndStep(value.z, &stepValue->z, &ceilValue->z);
}

//----------------------------------------------------------------------------------
void EffectRowNoise::CalcCeilAndStep(
    float value,
    float* stepValue,
    float* ceilValue)
{
  if (value == 0)
  {
    *stepValue = 0.1f;
    *ceilValue = 0;
    return;
  }

  float base = 10;
  float log10 = log(value) / log(base);
  *stepValue = pow(base, floor(log10));

  // step down from the ceil until we find the multiple of step just below
  float tmp = pow(base, ceil(log10));
  float step = *stepValue;
  while (true)
  {
    if (tmp - step < value)
      break;
    tmp -= step;
  }

  *ceilValue = tmp + step;
}

//----------------------------------------------------------------------------------
void EffectRowNoise::DrawGraph(RenderTexture& texture, const Vector2f& size)
{
  const editor::protocol::Settings& settings = EDITOR.Settings();
  int ofs = settings.effect_view_width();

  vector<pair<Vector2f, Vector3Keyframe*>> keyframes;
  VisibleKeyframes(size, true, &keyframes);

  Text label;
  label.setFont(_font);
  label.setCharacterSize(16);

  // draw min/max lines
  VertexArray gridLines(sf::Lines);
  float t0 = ExtractGraphValue(_realMaxValue);
  float step, maxValue;
  CalcCeilAndStep(t0, &step, &maxValue);
  float minValue = ExtractGraphValue(_realMinValue);

  Color c(100, 100, 100, 255);
  label.setColor(c);

  float curY = maxValue;
  while (true)
  {
    if (curY < minValue - step)
      break;

    float y = CalcGraphValue(Vector3f(curY, curY, curY));
    gridLines.append(sf::Vertex(Vector2f(ofs, y), c));
    gridLines.append(sf::Vertex(Vector2f(size.x, y), c));

    label.setPosition(ofs + 10, y - 20);
    label.setString(to_string("%.2f", curY).c_str());
    texture.draw(label);

    curY -= step;
  }

  texture.draw(gridLines);

  // draw the keyframes normalized to the min/max values
  VertexArray curLine(sf::LinesStrip);

  for (const auto& pp : keyframes)
  {
    const Vector2f& p = pp.first;
    curLine.append(sf::Vertex(p));

    // if the point corresponds to a proper keyframe, draw the keyframe
    if (pp.second)
    {
      _keyframeRect->_shape.setPosition(p.x - 3, p.y - 3);
      texture.draw(_keyframeRect->_shape);
    }
  }

  texture.draw(curLine);
}

//----------------------------------------------------------------------------------
void EffectRowNoise::BeginEditVars(float x, float y)
{
#if 0
  const editor::protocol::Settings& settings = EDITOR.Settings();
  int h = settings.effect_row_height();
  _prevValue = _selectedKeyframe
      ? _selectedKeyframe->value
      : Interpolate<Vector3f>(_effector.displacement, EDITOR.CurTime());

  _editingIdx = (y - _rect->_shape.getPosition().y - h) / h;
  assert(_editingIdx >= 0 && _editingIdx <= 2);

  Vector3f v = Interpolate<Vector3f>(_effector.displacement, EDITOR.CurTime());
  switch (_editingIdx)
  {
    case 0: _curEdit = to_string("%.2f", v.x); break;
    case 1: _curEdit = to_string("%.2f", v.y); break;
    case 2: _curEdit = to_string("%.2f", v.z); break;
  }
  #endif
}

//----------------------------------------------------------------------------------
void EffectRowNoise::EndEditVars(bool commit)
{
#if 0
  if (commit)
  {
    // create a new keyframe at the current position (if one doesn't exist)
    Vector3f v = _prevValue;
    switch (_editingIdx)
    {
      case 0: v.x = (float)atof(_curEdit.c_str()); break;
      case 1: v.y = (float)atof(_curEdit.c_str()); break;
      case 2: v.z = (float)atof(_curEdit.c_str()); break;
    }

    AddKeyframe(EDITOR.CurTime(), v, false, &_effector.displacement);
  }

  _editingIdx = -1;
 #endif
}

//----------------------------------------------------------------------------------
void EffectRowNoise::UpdateEditVar(Keyboard::Key key)
{
  if (key >= Keyboard::Key::Num0 && key <= Keyboard::Key::Num9)
  {
    _curEdit += '0' + key - Keyboard::Key::Num0;
  }
  else if (key == Keyboard::Key::BackSpace)
  {
    if (_curEdit.size() > 0)
    {
      _curEdit.pop_back();
    }
  }
  else if (key == Keyboard::Key::Period && _curEdit.find('.') == _curEdit.npos)
  {
    _curEdit += '.';
  }
}

//----------------------------------------------------------------------------------
bool EffectRowNoise::KeyframeIntersect(const Vector2f& pt, const Vector2f& size)
{
#if 0
  const editor::protocol::Settings& settings = EDITOR.Settings();

  float shapeY = _rect->_shape.getPosition().y;
  float h = settings.effect_row_height();

  for (u32 i = 0; i < _effector.displacement.keyframe.size(); ++i)
  {
    Vector3Keyframe& keyframe = _effector.displacement.keyframe[i];
    int x = TimelineWindow::_instance->TimeToPixel(milliseconds(keyframe.time));
    if (x >= size.x)
      continue;

    if (_graphMode == 0)
    {
      // keyframe mode; check each var
      for (u32 j = 0; j < 3; ++j)
      {
        int y = shapeY + h * (j+1.5f) - 3;
        FloatRect rect(x, y, 6, 6);
        if (rect.contains(pt))
        {
          _selectedKeyframe = &keyframe;
          _oldKeyframe = keyframe;
          return true;
        }
      }
    }
    else
    {
      // graph mode; check the currect var
      float y = ValueToPixel(keyframe.value);
      FloatRect rect(x, y, 6, 6);
      if (rect.contains(pt))
      {
        _selectedKeyframe = &keyframe;
        _oldKeyframe = keyframe;
        return true;
      }
    }

  }
#endif
  return false;
}

//----------------------------------------------------------------------------------
void EffectRowNoise::UpdateKeyframe(const time_duration &t)
{
  if (!_selectedKeyframe)
    return;

  _selectedKeyframe->time = t.total_milliseconds();
}

//----------------------------------------------------------------------------------
void EffectRowNoise::BeginKeyframeUpdate(bool copy)
{
#if 0
  _copyingKeyframe = copy;
  if (_copyingKeyframe)
  {
    // insert a copy of the existing keyframe
    _selectedKeyframe = AddKeyframe(
        milliseconds(_selectedKeyframe->time),
        _selectedKeyframe->value,
        true,
        &_effector.displacement);
  }
#endif
}

//----------------------------------------------------------------------------------
void EffectRowNoise::EndKeyframeUpdate(bool commit)
{
  if (!commit)
  {
    if (_copyingKeyframe)
    {
      // delete the tmp keyframe
      _selectedKeyframe = nullptr;
    }
    else
    {
      // revert the old keyframe
      *_selectedKeyframe = _oldKeyframe;
    }
  }
}

//----------------------------------------------------------------------------------
void EffectRowNoise::DeselectKeyframe()
{
  _selectedKeyframe = nullptr;
}

//----------------------------------------------------------------------------------
void EffectRowNoise::DeleteKeyframe()
{
  if (!_selectedKeyframe)
    return;
}

//----------------------------------------------------------------------------------
bool EffectRowNoise::NextGraph()
{
  _graphMode = (_graphMode + 1) % 4;
  return _graphMode == 0;
}

//----------------------------------------------------------------------------------
void EffectRowNoise::ToggleGraphView(bool value)
{
  // reset the first graph or to keyframe view
  _graphMode = value ? 1 : 0;
}

//----------------------------------------------------------------------------------
float EffectRowNoise::SnappedValue(float value)
{
  return value;
}

//----------------------------------------------------------------------------------
bool EffectRowNoise::OnMouseMoved(const Event &event)
{
  TimelineWindow* timeline = TimelineWindow::_instance;

  int x = (int)(event.mouseMove.x - timeline->GetPosition().x);
  int y = (int)(event.mouseMove.y - timeline->GetPosition().y);
  Vector2i mousePos(x, y);

  if (_selectedKeyframe)
  {
    const Vector3f& v = PixelToValue(y);
    _selectedKeyframe->time = timeline->PixelToTime(x).total_milliseconds();
    _selectedKeyframe->value = UpdateKeyframe(v, _selectedKeyframe->value);
    timeline->KeyframesModified();

    timeline->UpdateStatusBar(0, to_string("Value: %.3f", ExtractGraphValue(v)));
  }
  return true;
}

//----------------------------------------------------------------------------------
Vector3f EffectRowNoise::UpdateKeyframe(const Vector3f& newValue, const Vector3f& old) const
{
  Vector3f res = old;
  switch (_graphMode)
  {
    case 1: res.x = newValue.x; break;
    case 2: res.y = newValue.y; break;
    case 3: res.z = newValue.z; break;
  }

  return res;
}

//----------------------------------------------------------------------------------
bool EffectRowNoise::OnMouseButtonPressed(const Event &event)
{
#if 0
  TimelineWindow* timeline = TimelineWindow::_instance;

  int x = (int)(event.mouseButton.x - timeline->GetPosition().x);
  int y = (int)(event.mouseButton.y - timeline->GetPosition().y);
  Vector2i mousePos(x, y);

  _selectedKeyframe = nullptr;

  // check for keyframe intersection
  vector<pair<Vector2f, Vector3Keyframe*>> keyframes;
  VisibleKeyframes(timeline->GetSize(), false, &keyframes);

  for (const auto& pp : keyframes)
  {
    const Vector2f& p = pp.first;

    IntRect rect(p.x - 3, p.y - 3, 6, 6);
    if (rect.contains(mousePos))
    {
      _selectedKeyframe = pp.second;
      break;
    }
  }

  // check if we should add a new keyframe
  if (!_selectedKeyframe && Keyboard::isKeyPressed(Keyboard::Key::LShift))
  {
    time_duration t = timeline->PixelToTime(x);
    Vector3f tmp = Interpolate<Vector3f>(_effector.displacement, t);
    AddKeyframe(t, UpdateKeyframe(PixelToValue(y), tmp), true, &_effector.displacement);
    timeline->KeyframesModified();
  }
#endif
  return true;
}

//----------------------------------------------------------------------------------
bool EffectRowNoise::OnMouseButtonReleased(const Event &event)
{
  _selectedKeyframe = nullptr;
  return true;
}

//----------------------------------------------------------------------------------
bool EffectRowNoise::FromProtocol(const google::protobuf::Message& proto)
{
  const effect::protocol::plexus::NoiseEffector& p = static_cast<const effect::protocol::plexus::NoiseEffector&>(proto);
  _effector = ::FromProtocol(p);
  return true;
}

//----------------------------------------------------------------------------------
bool EffectRowNoise::ToProtocol(google::protobuf::Message* msg) const
{
  effect::protocol::plexus::Plexus* proto = static_cast<effect::protocol::plexus::Plexus*>(msg);

  ::ToProtocol(_effector, proto->add_noise_effectors());
  return true;
}
