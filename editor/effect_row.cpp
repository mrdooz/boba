#include "effect_row.hpp"
#include "editor_windows.hpp"
#include "editor.hpp"

using namespace editor;
using namespace bristol;

//----------------------------------------------------------------------------------
EffectRow::EffectRow(
    const Font& font,
    const string& str,
    EffectRow* parent)
    : _str(str)
    , _parent(parent)
    , _level(0)
    , _font(font)
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
void EffectRow::Flatten(EffectRow* cur, vector<EffectRow*>* res)
{
  deque<EffectRow*> q({cur});
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
void EffectRow::Reposition(EffectRow* cur, float curY, float rowHeight)
{
  const editor::protocol::Settings& settings = EDITOR.Settings();
  const Vector2f windowSize = TimelineWindow::_instance->GetSize();

  deque<EffectRow*> q({ cur });
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

    u32 numVars = cur->NumVars();
    if (numVars > 0)
    {
      float x = cur->_level * 15 + shape.getPosition().x;
      cur->_varEditRect = FloatRect(
          x,
          shape.getPosition().y + 1 * rowHeight,
          (settings.effect_view_width() - x) / 2,
          numVars * rowHeight);
    }
  }
}

//----------------------------------------------------------------------------------
float EffectRow::RowHeight(EffectRow* cur, float rowHeight)
{
  float res = 0;
  deque<EffectRow*> q({ cur });
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
  Color rowCol = FromProtocol(_flags.IsSet(EffectRow::RowFlagsF::Selected)
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
    // draw expanded indicator

    // y increases downwards
    const Style* s = STYLE_FACTORY.GetStyle("effect_icon_collapsed_color");
    tri.append(sf::Vertex(Vector2f(left+5, y+5), s->fillColor));
    tri.append(sf::Vertex(Vector2f(left+5, y+15), s->fillColor));
    tri.append(sf::Vertex(Vector2f(left+15, y+10), s->fillColor));
    texture.draw(tri);
  }
  else
  {
    // y increases downwards
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
bool EffectRowPlexus::ToProtocol(effect::protocol::EffectSetting* proto) const
{
  assert(!_parent);
  proto->set_type(effect::protocol::EffectSetting_Type_Plexus);

  effect::protocol::plexus::Plexus plexus;
  for (const EffectRowTextPath* row : _textPaths)
  {
    row->ToProtocolInner(&plexus);
  }

  for (const EffectRowNoise* row : _noise)
  {
    row->ToProtocolInner(&plexus);
  }

  proto->set_msg(plexus.SerializeAsString());

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
bool EffectRowTextPath::ToProtocolInner(effect::protocol::plexus::Plexus* proto) const
{
  proto->add_text_paths()->set_text(_str);
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
}

//----------------------------------------------------------------------------------
void EffectRowNoise::DrawVars(RenderTexture& texture, bool drawKeyframes)
{
  Vector2f size = TimelineWindow::_instance->GetSize();
  const editor::protocol::Settings& settings = EDITOR.Settings();
  float h = settings.effect_row_height();

  Vector3f v = Interpolate(_effector.displacement, EDITOR.CurTime().total_milliseconds());

  const Style* editingStyle = STYLE_FACTORY.GetStyle("var_text_editing");
  const Style* normalStyle = STYLE_FACTORY.GetStyle("var_text_normal");

  static const char* suffix[] = {
      "x: ",
      "y: ",
      "z: "
  };

  const auto& fnStyle = [this, editingStyle, normalStyle, v](int curIdx){
      if (curIdx == _editingIdx)
      {
        _text.setString(to_string("%s%s", suffix[curIdx], _curEdit.c_str()).c_str());
        _text.setColor(editingStyle->fillColor);
        _text.setStyle(editingStyle->fontStyle);
      }
      else
      {
        _text.setString(to_string("%s%.2f", suffix[curIdx], *(&v.x + curIdx)).c_str());
        _text.setColor(normalStyle->fillColor);
        _text.setStyle(normalStyle->fontStyle);
      }
  };

  fnStyle(0);
  _text.setPosition(_rect->_shape.getPosition() + Vector2f(20 + _level * 15, h*1));
  texture.draw(_text);

  fnStyle(1);
  _text.setPosition(_rect->_shape.getPosition() + Vector2f(20 + _level * 15, h*2));
  texture.draw(_text);

  fnStyle(2);
  _text.setPosition(_rect->_shape.getPosition() + Vector2f(20 + _level * 15, h*3));
  texture.draw(_text);

  if (drawKeyframes)
  {
    DrawKeyframes(texture, size);
  }

  // Reset style
  fnStyle(-2);
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

  for (u32 i = 0; i < _effector.displacement.keyframes.size(); ++i)
  {
    const Vector3Keyframe& keyframe = _effector.displacement.keyframes[i];

    ApplyStyle(STYLE_FACTORY.GetStyle(&keyframe == _selectedKeyframe ?
        "keyframe_style_selected" : "keyframe_style"), &_keyframeRect->_shape);

    int x = TimelineWindow::_instance->TimeToPixel(milliseconds(keyframe.time));
    if (x < size.x)
    {
      // draw for each var
      for (u32 j = 0; j < 3; ++j)
      {
        int y = shapeY + h * (j+1.5f) - 3;
        _keyframeRect->_shape.setPosition(x - 3, y);
        texture.draw(_keyframeRect->_shape);
      }
    }
  }
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
  const editor::protocol::Settings& settings = EDITOR.Settings();
  float ofs = settings.effect_view_width();

  TimelineWindow* timeline = TimelineWindow::_instance;

  // find min/max values for keyframes within the view
  time_duration minTime = timeline->PixelToTime(0);
  time_duration maxTime = timeline->PixelToTime(size.x);

  Vector3f value0 = Interpolate(_effector.displacement, minTime);
  Vector3f valueLast = Interpolate(_effector.displacement, maxTime);

  vector<Vector3Keyframe*> validKeyframes;

  if (!_selectedKeyframe)
  {
    _minValue = Min(
        Interpolate(_effector.displacement, minTime),
        Interpolate(_effector.displacement, maxTime));

    _maxValue = Max(
        Interpolate(_effector.displacement, minTime),
        Interpolate(_effector.displacement, maxTime));


    for (Vector3Keyframe& keyframe : _effector.displacement.keyframes)
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
    for (Vector3Keyframe& keyframe : _effector.displacement.keyframes)
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
    *stepValue = 0.1;
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
u32 EffectRowNoise::NumVars()
{
  return 3;
}

//----------------------------------------------------------------------------------
void EffectRowNoise::BeginEditVars(float x, float y)
{
  const editor::protocol::Settings& settings = EDITOR.Settings();
  int h = settings.effect_row_height();
  _prevValue = _selectedKeyframe
      ? _selectedKeyframe->value
      : Interpolate(_effector.displacement, EDITOR.CurTime());

  _editingIdx = (y - _rect->_shape.getPosition().y - h) / h;
  assert(_editingIdx >= 0 && _editingIdx <= 2);

  Vector3f v = Interpolate(_effector.displacement, EDITOR.CurTime());
  switch (_editingIdx)
  {
    case 0: _curEdit = to_string("%.2f", v.x); break;
    case 1: _curEdit = to_string("%.2f", v.y); break;
    case 2: _curEdit = to_string("%.2f", v.z); break;
  }
}

//----------------------------------------------------------------------------------
void EffectRowNoise::EndEditVars(bool commit)
{
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
  const editor::protocol::Settings& settings = EDITOR.Settings();

  float shapeY = _rect->_shape.getPosition().y;
  float h = settings.effect_row_height();

  for (u32 i = 0; i < _effector.displacement.keyframes.size(); ++i)
  {
    Vector3Keyframe& keyframe = _effector.displacement.keyframes[i];
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
    Vector3f tmp = Interpolate(_effector.displacement, t);
    AddKeyframe(t, UpdateKeyframe(PixelToValue(y), tmp), true, &_effector.displacement);
    timeline->KeyframesModified();
  }

  return true;
}

//----------------------------------------------------------------------------------
bool EffectRowNoise::OnMouseButtonReleased(const Event &event)
{
  _selectedKeyframe = nullptr;
  return true;

}

//----------------------------------------------------------------------------------
bool EffectRowNoise::ToProtocolInner(effect::protocol::plexus::Plexus* proto) const
{
  ::ToProtocol(_effector, proto->add_noise_effectors());
  return true;
}
