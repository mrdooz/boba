#include "effect_row.hpp"
#include "editor_windows.hpp"
#include "editor.hpp"

using namespace editor;
using namespace bristol;

#pragma warning(disable: 4244)


//----------------------------------------------------------------------------------
void CalcCeilAndStep(float value, float* stepValue, float* ceilValue)
{
  if (value == 0)
  {
    *stepValue = 0.1f;
    *ceilValue = 0;
    return;
  }

  float base = 10;
  float log10 = logf(value) / logf(base);
  *stepValue = powf(base, floorf(log10));

  // step down from the ceil until we find the multiple of step just below
  float tmp = powf(base, ceilf(log10));
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
void DrawRow(RenderTexture& texture, float x, float y, float w, float h, const Color& color)
{
  VertexArray curLine(sf::Lines);
  curLine.append(sf::Vertex(Vector2f(x, y), color));
  curLine.append(sf::Vertex(Vector2f(w, y), color));
  curLine.append(sf::Vertex(Vector2f(x, y+h), color));
  curLine.append(sf::Vertex(Vector2f(w, y+h), color));
  texture.draw(curLine);
}

//----------------------------------------------------------------------------------
RowVar::RowVar(
    const Font& font,
    const string& name,
    FloatAnim* anim)
    : _font(font)
    , _name(name)
    , _value(0)
    , _anim(anim)
    , _selectedKeyframe(~0)
{
  _text.setFont(font);
  _text.setCharacterSize(16);
}

//----------------------------------------------------------------------------------
void RowVar::Draw(RenderTexture& texture, bool drawKeyframes)
{
  const editor::protocol::Settings& settings = EDITOR.Settings();
  const Vector2f& windowSize = TimelineWindow::_instance->GetSize();

  Color bgCol = ::FromProtocol(_flags.IsSet(VarFlagsF::Selected)
      ? settings.effect_view_background_color_selected()
      : settings.effect_view_background_color());

  RectangleShape bgRect;
  bgRect.setPosition(_bounds.left, _bounds.top);
  bgRect.setSize(Vector2f(_bounds.width, _bounds.height));
  bgRect.setFillColor(bgCol);
  texture.draw(bgRect);

  _text.setString("A");
  if (_flags.IsSet(VarFlagsF::Animating))
  {
    _text.setStyle(sf::Text::Bold);
    _text.setColor(Color(200, 200, 0));
  }
  else
  {
    _text.setStyle(sf::Text::Regular);
    _text.setColor(Color(200, 200, 200));
  }
  _text.setPosition(_bounds.left, _bounds.top);
  texture.draw(_text);

  const StyleSetting* style = _flags.IsSet(VarFlagsF::Editing)
    ? STYLE_FACTORY.GetStyle("var_text_editing")
    : STYLE_FACTORY.GetStyle("var_text_normal");

  _text.setString(to_string("%s: %.2f", _name.c_str(), _value).c_str());
  _text.setColor(style->fillColor);
  _text.setStyle(style->fontStyle);

  Vector2f pos(_bounds.left + 20, _bounds.top);
  _text.setPosition(pos.x, pos.y);
  texture.draw(_text);

  DrawRow(texture, 0, pos.y, drawKeyframes ? windowSize.x : settings.effect_view_width(),
      _bounds.height, ::FromProtocol(settings.effect_line_color()));
}

//----------------------------------------------------------------------------------
Vector2f RowVar::ToLocal(int x, int y) const
{
  TimelineWindow* timeline = TimelineWindow::_instance;

  const Vector2f& windowPos = timeline->GetPosition();
  int xx = (int)(x - windowPos.x);
  int yy = (int)(y - windowPos.y);
  return Vector2f(xx, yy);
}

//----------------------------------------------------------------------------------
void RowVar::DrawKeyframes(RenderTexture& texture)
{
  _flags.Clear(VarFlagsF::GraphMode);

  const editor::protocol::Settings& settings = EDITOR.Settings();
  float h = settings.effect_row_height();
  int s = settings.keyframe_size();
  float ofs = (h - s) / 2;
  float y = _bounds.top;

  const StyleSetting* defaultStyle = STYLE_FACTORY.GetStyle("keyframe_style");
  const StyleSetting* selectedStryle = STYLE_FACTORY.GetStyle("keyframe_style_selected");

  _keyframeRect._rect.setSize(Vector2f(settings.keyframe_size(), settings.keyframe_size()));

  // draw the keyframes
  const Vector2f& windowSize = TimelineWindow::_instance->GetSize();
  for (u32 i = 0; i < _anim->keyframe.size(); ++i)
  {
    const FloatKeyframe& keyframe = _anim->keyframe[i];
    int keyX = TimelineWindow::_instance->TimeToPixel(milliseconds(keyframe.key.x));
    if (keyX < windowSize.x)
    {
      ApplyStyle(i == _selectedKeyframe ? selectedStryle : defaultStyle, &_keyframeRect._rect);

      _keyframeRect._rect.setPosition(keyX - s/2, y + ofs);
      texture.draw(_keyframeRect._rect);
    }
  }
}

//----------------------------------------------------------------------------------
bool RowVar::OnMouseButtonPressed(const Event &event)
{
  TimelineWindow* timeline = TimelineWindow::_instance;
  const editor::protocol::Settings& settings = EDITOR.Settings();
  Vector2f mousePos = ToLocal(event.mouseButton.x, event.mouseButton.y);
  float x = mousePos.x, y = mousePos.y;

  _selectedKeyframe = ~(u32)0;

  if (y >= _bounds.top && y < _bounds.top + _bounds.height)
  {
    // check for animation toggle
    FloatRect r(_bounds.left, _bounds.top, 10, _bounds.height);
    if (r.contains(mousePos))
    {
      _flags.Toggle(VarFlagsF::Animating);
      return true;
    }

    // check for variable edit
    r.left += 15;
    r.width = 50;
    if (r.contains(mousePos))
    {
      _flags.Set(VarFlagsF::PreEdit);
      return true;
    }

    // check for hit on the var window
    if (_bounds.contains(mousePos))
    {
      timeline->SendEffectEvent(this, EffectRowEvent(EffectRowEvent::Type::VarSelected));
      _flags.Toggle(VarFlagsF::Selected);
      return true;
    }
  }

  int ofs = settings.keyframe_size();
  int ofs2 = ofs / 2;

  if (_flags.IsSet(VarFlagsF::GraphMode))
  {
    // if shift is pressed, create a new control point at the cursor pos
    if (Keyboard::isKeyPressed(Keyboard::Key::LShift))
    {
      time_duration t = timeline->PixelToTime(event.mouseButton.x);
      float y = event.mouseButton.y - timeline->GetPosition().y;
      float v = PixelToValue(y);
      AddKeyframe<float>(t, v, true, _anim);
      return true;
    }

    if (_anim->type == 0)
    {
      for (u32 i = 0; i < _anim->keyframe.size(); ++i)
      {
        FloatKeyframe &keyframe = _anim->keyframe[i];
        FloatRect rect(
            timeline->TimeToPixel(milliseconds(keyframe.key.x)),
            ValueToPixel(keyframe.key.y), ofs, ofs);
        if (rect.contains(mousePos))
        {
          _selectedKeyframe = i;
          _prevKeyframe = keyframe;
          return true;
        }
      }
    }
    else
    {
      // if in bezier, check for hits on control points
      for (u32 i = 0; i < _anim->keyframe.size(); ++i)
      {
        FloatKeyframe &keyframe = _anim->keyframe[i];
        FloatRect rect(
            timeline->TimeToPixel(milliseconds(keyframe.key.x)),
            ValueToPixel(keyframe.key.y), ofs, ofs);
        if (rect.contains(mousePos))
        {
          _selectedKeyframe = i;
          _prevKeyframe = keyframe;
          return true;
        }

        FloatRect rectIn(
            timeline->TimeToPixel(milliseconds(keyframe.cpIn.x)),
            ValueToPixel(keyframe.cpIn.y), ofs, ofs);
        if (rectIn.contains(mousePos))
        {
          _selectedKeyframe = i | (1 << 31);
          _prevKeyframe = keyframe;
          return true;
        }

        FloatRect rectOut(
            timeline->TimeToPixel(milliseconds(keyframe.cpOut.x)),
            ValueToPixel(keyframe.cpOut.y), ofs, ofs);
        if (rectOut.contains(mousePos))
        {
          _selectedKeyframe = i | (1 << 30);
          _prevKeyframe = keyframe;
          return true;
        }
      }

    }
  }
  else
  {
    // check if this intersects any keyframe
    u32 t0 = timeline->PixelToTime(x - ofs2).total_milliseconds();
    u32 t1 = timeline->PixelToTime(x + ofs2).total_milliseconds();

    for (u32 i = 0; i < _anim->keyframe.size(); ++i)
    {
      FloatKeyframe& keyframe = _anim->keyframe[i];
      if (keyframe.key.x >= t0 && keyframe.key.x < t1)
      {
        _selectedKeyframe = i;
        return true;
      }
    }
  }

  return false;
}

//----------------------------------------------------------------------------------
bool RowVar::OnMouseButtonReleased(const Event &event)
{
  _selectedKeyframe = ~(u32)0;

  if (_flags.IsSet(VarFlagsF::Animating) && _flags.IsSet(VarFlagsF::Editing))
  {
    AddKeyframe<float>(EDITOR.CurTime(), _value, false, _anim);
    _flags.Clear(VarFlagsF::PreEdit);
    _flags.Clear(VarFlagsF::Editing);
  }

  return false;
}

//----------------------------------------------------------------------------------
bool RowVar::OnMouseMoved(const Event &event)
{
  Vector2f mousePos = ToLocal(event.mouseMove.x, event.mouseMove.y);
  TimelineWindow* timeline = TimelineWindow::_instance;

  if (_selectedKeyframe != ~0)
  {
    vector<FloatKeyframe>& keyframes = _anim->keyframe;

    float t = timeline->PixelToTime(mousePos.x).total_milliseconds();

    if (_flags.IsSet(VarFlagsF::GraphMode))
    {
      if (_anim->type == 0)
      {
        // if moving past the previous or next keyframe, swap to it
        if (_selectedKeyframe > 0 && t < keyframes[_selectedKeyframe-1].key.x)
        {
          keyframes[_selectedKeyframe] = _prevKeyframe;
          _prevKeyframe = keyframes[--_selectedKeyframe];
        }

        if (_selectedKeyframe < keyframes.size() -1 && t >= keyframes[_selectedKeyframe+1].key.x)
        {
          keyframes[_selectedKeyframe] = _prevKeyframe;
          _prevKeyframe = keyframes[++_selectedKeyframe];
        }

        float y = event.mouseMove.y - timeline->GetPosition().y;
        keyframes[_selectedKeyframe].key.x = t;
        keyframes[_selectedKeyframe].key.y = PixelToValue(y);
      }
      else
      {
        // bezier
        float y = event.mouseMove.y - timeline->GetPosition().y;
        u32 k = _selectedKeyframe & ~(0x3 << 30);

        if (_selectedKeyframe & (1 << 31))
        {
          keyframes[k].cpIn.x = t;
          keyframes[k].cpIn.y = PixelToValue(y);
        }
        else if (_selectedKeyframe & (1 << 30))
        {
          keyframes[k].cpOut.x = t;
          keyframes[k].cpOut.y = PixelToValue(y);
        }
        else
        {
          float dx = t - keyframes[k].key.x;
          float dy = PixelToValue(y) - keyframes[k].key.y;
          keyframes[k].key.x = t;
          keyframes[k].key.y = PixelToValue(y);

          keyframes[k].cpIn.x += dx;
          keyframes[k].cpIn.y += dy;

          keyframes[k].cpOut.x += dx;
          keyframes[k].cpOut.y += dy;
        }
      }
    }
    else
    {
      // don't allow the keyframe to move outside its neighbours
      if (_selectedKeyframe > 0)
        t = max(t, keyframes[_selectedKeyframe-1].key.x);

      if (_selectedKeyframe < keyframes.size() -1)
        t = min(t, keyframes[_selectedKeyframe+1].key.x);

      keyframes[_selectedKeyframe].key.x = t;
    }

    return true;
  }

  if (_flags.IsSet(VarFlagsF::PreEdit))
  {
    _flags.Clear(VarFlagsF::PreEdit);
    _flags.Set(VarFlagsF::Editing);
    _prevPos = mousePos;
  }

  if (_flags.IsSet(VarFlagsF::Editing))
  {
    Vector2f delta = mousePos - _prevPos;
    _prevPos = mousePos;
    _value += delta.x;
    return true;
  }

  return false;
}

//----------------------------------------------------------------------------------
void RowVar::OnEvent(RowVar* sender, const EffectRowEvent& event)
{
  if (event.type == EffectRowEvent::Type::VarSelected)
  {
    _flags.Clear(VarFlagsF::Selected);
    _flags.Clear(VarFlagsF::Editing);
    _flags.Clear(VarFlagsF::Animating);
    _flags.Clear(VarFlagsF::PreEdit);
  }
}

//----------------------------------------------------------------------------------
float RowVar::PixelToValue(float y) const
{
  // x = bottom - h * (value - minValue) / span
  // x - bottom = -h..
  // ==> value = (bottom - x) * span / h + minValue

  Vector2f size = TimelineWindow::_instance->GetSize();
  const editor::protocol::Settings& settings = EDITOR.Settings();
  float topY = settings.ticker_height();
  float h = size.y - topY;
  float bottom = size.y - 1;

  float span = _maxValue - _minValue;
  return (bottom - y) * span / h + _minValue;
}

//----------------------------------------------------------------------------------
float RowVar::ValueToPixel(float value) const
{
  // x = bottom - h * (value - minValue) / span

  Vector2f size = TimelineWindow::_instance->GetSize();
  const editor::protocol::Settings& settings = EDITOR.Settings();
  float topY = settings.ticker_height();
  float h = size.y - topY;
  float bottom = size.y - 1;

  float span = _maxValue - _minValue;
  float v = value - _minValue;
  return bottom - h * v / span;
}

//----------------------------------------------------------------------------------
void RowVar::VisibleKeyframes(
    const Vector2f& size,
    u32 flags,
    vector<pair<Vector2f, const FloatKeyframe*>>* keyframes)
{

  const editor::protocol::Settings& settings = EDITOR.Settings();
  float ofs = settings.effect_view_width();

  TimelineWindow* timeline = TimelineWindow::_instance;

  // find min/max values for keyframes within the view
  time_duration minTime = timeline->PixelToTime(0);
  time_duration maxTime = timeline->PixelToTime(size.x);

  float value0 = Interpolate<float>(*_anim, minTime);
  float valueLast = Interpolate<float>(*_anim, maxTime);

  _minValue = min(value0, valueLast);
  _maxValue = max(value0, valueLast);

  vector<const FloatKeyframe*> validKeyframes;

  const FloatKeyframe* prevFrame = nullptr;
  float minMs = minTime.total_milliseconds();
  float maxMs = maxTime.total_milliseconds();

  for (u32 i = 0; i < _anim->keyframe.size(); ++i)
  {
    const FloatKeyframe& keyframe = _anim->keyframe[i];

    if (keyframe.key.x < minMs)
    {
      prevFrame = &keyframe;
      continue;
    }

    if (keyframe.key.x > maxMs)
    {
      if (flags & AddOutside)
        validKeyframes.push_back(&keyframe);
      break;
    }

    if (prevFrame && prevFrame->key.x < minMs && flags & AddOutside)
      validKeyframes.push_back(prevFrame);

    validKeyframes.push_back(&keyframe);

    _minValue = min(_minValue, keyframe.key.y);
    _maxValue = max(_maxValue, keyframe.key.y);

    prevFrame = &keyframe;
  }

  _realMinValue = _minValue;
  _realMaxValue = _maxValue;

  float step;
  CalcCeilAndStep(_maxValue, &step, &_maxValue);
  _minValue = _realMinValue - step;

  if (flags & AddBorderPoints)
    keyframes->push_back(make_pair(Vector2f(ofs, ValueToPixel(value0)), nullptr));

  for (const FloatKeyframe* keyframe : validKeyframes)
  {
    Vector2f p = Vector2f(
        timeline->TimeToPixel(milliseconds(keyframe->key.x)),
        ValueToPixel(keyframe->key.y));

    keyframes->push_back(make_pair(p, keyframe));
  }

  if (flags & AddBorderPoints)
    keyframes->push_back(make_pair(Vector2f(size.x, ValueToPixel(valueLast)), nullptr));
}

//----------------------------------------------------------------------------------
template <typename T>
T Bezier(const T& p0, const T& p1, const T& p2, const T& p3, float t)
{
  float t2 = t*t;
  float t3 = t2*t;

  float u = (1-t);
  float u2 = u*u;
  float u3 = u2*u;

  return u3 * p0 + 3 * u2 * t * p1 + 3 * u * t2 * p2 + t3 * p3;
}

//----------------------------------------------------------------------------------
void RowVar::DrawGraph(RenderTexture& texture)
{
  _flags.Set(VarFlagsF::GraphMode);

  const editor::protocol::Settings& settings = EDITOR.Settings();
  int ofs = settings.effect_view_width();

  TimelineWindow* timeline = TimelineWindow::_instance;
  Vector2f size = timeline->GetSize();

  // draw the background
  RectangleShape rect;
  rect.setPosition(settings.effect_view_width(), settings.ticker_height());
  rect.setSize(Vector2f(
      size.x - settings.effect_view_width(),
      size.y - settings.ticker_height()));

  rect.setFillColor(::FromProtocol(settings.timeline_view_background_color()));
  texture.draw(rect);

  vector<pair<Vector2f, const FloatKeyframe*>> keyframes;
  VisibleKeyframes(size, AddOutside, &keyframes);

  Text label;
  label.setFont(_font);
  label.setCharacterSize(16);

  VertexArray gridLines(sf::Lines);
  float t0 = _realMaxValue;
  float step, maxValue;
  CalcCeilAndStep(t0, &step, &maxValue);
  float minValue = _realMinValue;

  Color c(100, 100, 100, 255);
  label.setColor(c);

  // draw min/max lines
  float curY = maxValue;
  while (true)
  {
    if (curY < minValue - step)
      break;

    float y = ValueToPixel(curY);
    gridLines.append(sf::Vertex(Vector2f(ofs, y), c));
    gridLines.append(sf::Vertex(Vector2f(size.x, y), c));

    label.setPosition(ofs + 10, y - 20);
    label.setString(to_string("%.2f", curY).c_str());
    texture.draw(label);

    curY -= step;
  }

  texture.draw(gridLines);

  // draw the keyframes normalized to the min/max values

  if (_anim->type == 1)
  {
    VertexArray curLine(sf::LinesStrip);
    VertexArray controlPoints(sf::Lines);

    for (u32 i = 0; i < keyframes.size() - 1; ++i)
    {
      const FloatKeyframe* k0 = keyframes[i+0].second;
      const FloatKeyframe* k1 = keyframes[i+1].second;

      if (!(k0 && k1))
        continue;

      const Vector2f& p0 = Vector2f(
          timeline->TimeToPixel(milliseconds(k0->key.x)),
          ValueToPixel(k0->key.y));

      const Vector2f& p1 = Vector2f(
          timeline->TimeToPixel(milliseconds(k0->cpOut.x)),
          ValueToPixel(k0->cpOut.y));

      const Vector2f& p2 = Vector2f(
          timeline->TimeToPixel(milliseconds(k1->cpIn.x)),
          ValueToPixel(k1->cpIn.y));

      const Vector2f& p3 = Vector2f(
          timeline->TimeToPixel(milliseconds(k1->key.x)),
          ValueToPixel(k1->key.y));

      Color c(180, 200, 0);
      controlPoints.append(sf::Vertex(p0, c));
      controlPoints.append(sf::Vertex(p1, c));
      controlPoints.append(sf::Vertex(p2, c));
      controlPoints.append(sf::Vertex(p3, c));

      _keyframeRect._rect.setPosition(p0.x - 3, p0.y - 3);
      texture.draw(_keyframeRect._rect);

      _keyframeRect._rect.setPosition(p1.x - 3, p1.y - 3);
      texture.draw(_keyframeRect._rect);

      _keyframeRect._rect.setPosition(p2.x - 3, p2.y - 3);
      texture.draw(_keyframeRect._rect);

      _keyframeRect._rect.setPosition(p3.x - 3, p3.y - 3);
      texture.draw(_keyframeRect._rect);

      if (i == 0)
      {
        Vector2f v = Bezier(p0, p1, p2, p3, 0);
        curLine.append(sf::Vertex(v));
      }

      for (u32 j = 1; j <= 10; ++j)
      {
        float t = j / 10.0f;
        Vector2f v = Bezier(p0, p1, p2, p3, t);
        curLine.append(sf::Vertex(v));
      }
    }
    texture.draw(curLine);
    texture.draw(controlPoints);

  }
  else
  {
    VertexArray curLine(sf::LinesStrip);

    for (const auto& pp : keyframes)
    {
      const Vector2f& p = pp.first;
      curLine.append(sf::Vertex(p));

      // if the point corresponds to a proper keyframe, draw the keyframe
      if (pp.second)
      {
        _keyframeRect._rect.setPosition(p.x - 3, p.y - 3);
        texture.draw(_keyframeRect._rect);
      }
    }

    texture.draw(curLine);
  }

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
      var->_bounds = FloatRect(
          x,
          shape.getPosition().y + (i + 1) * rowHeight,
          settings.effect_view_width() - x,
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
bool EffectRow::OnMouseButtonPressed(const Event &event)
{
  const Vector2f& windowPos = TimelineWindow::_instance->GetPosition();
  int x = (int)(event.mouseButton.x - windowPos.x);
  int y = (int)(event.mouseButton.y - windowPos.y);
  Vector2f mousePos(x, y);

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

  if (_varEditRect.contains(mousePos))
  {
    BeginEditVars(x, y);
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
  else
  {
    for (RowVar* v : _vars)
    {
      v->OnMouseButtonReleased(event);
    }
  }
  return false;
}

//----------------------------------------------------------------------------------
bool EffectRow::OnKeyReleased(const Event& event)
{
//  Keyboard::Key code = event.key.code;

//  if (_flags.IsSet(RowFlagsF::Editing))
//  {
//    // escape aborts the pending operation
//    if (code == Keyboard::Escape)
//    {
//      EndEditVars(false);
//      EndKeyframeUpdate(false);
//    }
//
//    if (code == Keyboard::Return)
//    {
//      EndEditVars(true);
//    }
//    else
//    {
//      UpdateEditVar(event.key.code);
//    }
//
//    return true;
//  }

  return false;
}

//----------------------------------------------------------------------------------
void EffectRow::Draw(RenderTexture& texture, bool drawKeyframes)
{
  const editor::protocol::Settings& settings = EDITOR.Settings();
  Color bgCol = ::FromProtocol(settings.effect_view_background_color());

  // draw background
  float w = settings.effect_view_width();
  _rect._rect.setFillColor(bgCol);
  Vector2f size = _rect._rect.getSize();
  Vector2f windowSize = TimelineWindow::_instance->GetSize();
  _rect._rect.setSize(Vector2f(w, size.y));
  texture.draw(_rect._rect);

  // draw text
  _text.setString(_str);
  _text.setPosition(_rect._rect.getPosition() + Vector2f(20 + _level * 15, 0) );
  _text.setColor(::FromProtocol(settings.var_text_color()));
  texture.draw(_text);

  FloatRect bounds = _rect._rect.getGlobalBounds();
  DrawRow(texture, bounds.left, bounds.top, drawKeyframes ? windowSize.x : w, bounds.height, ::FromProtocol(settings.effect_line_color()));

  // draw expanded indicator
  VertexArray tri(sf::Triangles);
  float left = _level * 15 + _rect._rect.getPosition().x;
  float top = _rect._rect.getPosition().y;
  float y = top;

  if (!_flags.IsSet(EffectRow::RowFlagsF::Expanded))
  {
    const StyleSetting* s = STYLE_FACTORY.GetStyle("effect_icon_collapsed_color");
    // y increases downwards
    tri.append(sf::Vertex(Vector2f(left+5, y+5), s->fillColor));
    tri.append(sf::Vertex(Vector2f(left+5, y+15), s->fillColor));
    tri.append(sf::Vertex(Vector2f(left+15, y+10), s->fillColor));
    texture.draw(tri);
  }
  else
  {
    // Row is expanded, so draw the Vars and children
    const StyleSetting* s = STYLE_FACTORY.GetStyle("effect_icon_expanded_color");
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
void EffectRow::DrawVars(RenderTexture& texture, bool drawKeyframes)
{
  for (u32 i = 0; i < _vars.size(); ++i)
  {
    RowVar* var = _vars[i];
    var->Draw(texture, drawKeyframes);
    if (drawKeyframes)
      var->DrawKeyframes(texture);
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
  const effect::protocol::plexus::Plexus& p =
      static_cast<const effect::protocol::plexus::Plexus&>(proto);

  for (const effect::protocol::plexus::TextPath& textPath : p.text_paths())
  {
    string str = to_string("TextPath: %s", textPath.text().c_str());

    _children.push_back(new EffectRowTextPath(_font, str, this));
    _children.back()->FromProtocol(textPath);
  }

  for (const effect::protocol::plexus::NoiseEffector& effector : p.noise_effectors())
  {
    string str = to_string("Noise (%s)",
      effector.apply_to() == effect::protocol::plexus::NoiseEffector_ApplyTo_Position
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
{
  _vars.push_back(new RowVar(_font, "x", &_effector.displacement.x));
  _vars.push_back(new RowVar(_font, "y", &_effector.displacement.y));
  _vars.push_back(new RowVar(_font, "z", &_effector.displacement.z));
}

////----------------------------------------------------------------------------------
//void EffectRowNoise::UpdateEditVar(Keyboard::Key key)
//{
//  if (key >= Keyboard::Key::Num0 && key <= Keyboard::Key::Num9)
//  {
//    _curEdit += '0' + key - Keyboard::Key::Num0;
//  }
//  else if (key == Keyboard::Key::BackSpace)
//  {
//    if (_curEdit.size() > 0)
//    {
//      _curEdit.pop_back();
//    }
//  }
//  else if (key == Keyboard::Key::Period && _curEdit.find('.') == _curEdit.npos)
//  {
//    _curEdit += '.';
//  }
//}

//----------------------------------------------------------------------------------
void EffectRowNoise::UpdateKeyframe(const time_duration &t)
{
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

}

//----------------------------------------------------------------------------------
void EffectRowNoise::DeselectKeyframe()
{
}

//----------------------------------------------------------------------------------
void EffectRowNoise::DeleteKeyframe()
{
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
