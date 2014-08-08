#include "editor_windows.hpp"
#include "editor.hpp"

#include "protocol/effects_proto.hpp"
#import "flags.hpp"

using namespace editor;
using namespace bristol;

#pragma warning(disable: 4244)

//----------------------------------------------------------------------------------
PropertyWindow::PropertyWindow(
  const string& title,
  const Vector2f& pos,
  const Vector2f& size)
    : VirtualWindow(title, pos, size, bristol::WindowFlags(bristol::WindowFlag::StaticWindow))
{
}

//----------------------------------------------------------------------------------
PreviewWindow::PreviewWindow(
  const string& title,
  const Vector2f& pos,
  const Vector2f& size)
    : VirtualWindow(title, pos, size, bristol::WindowFlags(bristol::WindowFlag::StaticWindow))
{
}

TimelineWindow* TimelineWindow::_instance;

//----------------------------------------------------------------------------------
TimelineWindow::TimelineWindow(
  const string& title,
  const Vector2f& pos,
  const Vector2f& size)
    : VirtualWindow(title, pos, size, bristol::WindowFlags(bristol::WindowFlag::StaticWindow))
    , _panelOffset(seconds(0))
    , _pixelsPerSecond(EDITOR.Settings().timeline_zoom_default())
    , _lastDragPos(-1, -1)
    , _tickerRect(nullptr)
    , _movingKeyframe(nullptr)
{
  _instance = this;
}

//----------------------------------------------------------------------------------
TimelineWindow::~TimelineWindow()
{
}

//----------------------------------------------------------------------------------
bool TimelineWindow::Init()
{
  if (!VirtualWindow::Init())
    return false;

  _windowManager->RegisterHandler(Event::MouseButtonPressed, nullptr, bind(&TimelineWindow::OnMouseButtonPressed, this, _1));
  _windowManager->RegisterHandler(Event::MouseMoved, nullptr, bind(&TimelineWindow::OnMouseMoved, this, _1));
  _windowManager->RegisterHandler(Event::MouseButtonReleased, nullptr, bind(&TimelineWindow::OnMouseButtonReleased, this, _1));
  _windowManager->RegisterHandler(Event::MouseWheelMoved, nullptr, bind(&TimelineWindow::OnMouseWheelMoved, this, _1));
  _windowManager->RegisterHandler(Event::KeyReleased, nullptr, bind(&TimelineWindow::OnKeyReleased, this, _1));

  if (!_font.loadFromFile(EDITOR.GetAppRoot() + "gfx/coders_crux.ttf"))
    return false;

  const editor::protocol::Settings& settings = EDITOR.Settings();
  int width = settings.effect_view_width();
  int height = 20;

  // create render textures and sprites
//  _effectTexture.create(settings.effect_view_width(), _size.y);
//  _effectSprite.setTexture(_effectTexture.getTexture(), true);
//
//  _timelineTexture.create(_size.x - settings.effect_view_width(), _size.y);
//  _timelineSprite.setTexture(_timelineTexture.getTexture(), true);
//  _timelineSprite.setPosition(settings.effect_view_width(), 0);

  int rowHeight = settings.effect_row_height();
  int curY = settings.ticker_height();

  _tickerRect = STYLE_FACTORY.CreateStyledRectangle("default_row_color");
  _tickerRect->_shape.setPosition(width, 0);
  _tickerRect->_shape.setSize(Vector2f(_size.x - width, settings.ticker_height()));

  // create the effect rows
  const Plexus& p = EDITOR._plexus;

  for (int i = 0; i < 5; ++i)
  {
    EffectRow* parent = new EffectRow(_font, "PLEXUS");
    _effectRows.push_back(parent);
    curY += rowHeight;

    for (const TextPath& t : p.textPaths)
    {
      string str = to_string("TextPath: %s", t.text.c_str());
      parent->children.push_back(new EffectRow(_font, str, parent));
      curY += rowHeight;
    }

    for (const NoiseEffector& e : p.noiseEffectors)
    {
      string str = to_string("Noise (%s)",
        e.applyTo == NoiseEffector::ApplyTo::Position ? "POS" : "SCALE");
      EffectRowNoise* n = new EffectRowNoise(_font, str, parent);
      n->effector = e;
      parent->children.push_back(n);
      curY += rowHeight;
    }
  }

  RecalcEffecRows();

  return true;
}

//----------------------------------------------------------------------------------
void TimelineWindow::RecalcEffecRows()
{
  const editor::protocol::Settings& settings = EDITOR.Settings();
  float rowHeight = settings.effect_row_height();

  float curY = settings.ticker_height();

  for (EffectRow* row : _effectRows)
  {
    EffectRow::Reposition(row, curY, rowHeight);
    curY += EffectRow::RowHeight(row, rowHeight);
  }
}

//----------------------------------------------------------------------------------
bool TimelineWindow::OnKeyReleased(const Event &event)
{
  if (_editRow)
  {
    Keyboard::Key code = event.key.code;

    if (code == Keyboard::Return)
    {
      _editRow->EndEditVars(false);
    }
    else if (code == Keyboard::Escape)
    {
      _editRow->EndEditVars(true);
    }
    else
    {
      _editRow->UpdateEditVar(event.key.code);
    }
  }
  return true;
}

//----------------------------------------------------------------------------------
bool TimelineWindow::OnMouseButtonPressed(const Event& event)
{
  const editor::protocol::Settings& settings = EDITOR.Settings();

  int x = (int)(event.mouseButton.x - _pos.x);
  int y = (int)(event.mouseButton.y - _pos.y);
  Vector2f mousePos(x, y);

  vector<EffectRow*> effects;
  for (EffectRow* row : _effectRows)
    EffectRow::Flatten(row, &effects);

  if (y < (int)settings.ticker_height())
  {
    time_duration t = PixelToTime(event.mouseButton.x - (int)_pos.x);
    if (!t.is_not_a_date_time())
    {
      EDITOR.SetCurTime(t);
    }
  }
  else if (x < (int)settings.effect_view_width())
  {

    EffectRow* rowHit = nullptr;
    for (EffectRow* row : effects)
    {
      if (row->expandRect.contains(mousePos))
      {
        row->flags.Toggle(EffectRow::RowFlagsF::Expanded);
        rowHit = row;
        break;
      }
      else if (row->varEditRect.contains(mousePos))
      {
        row->InitVarEdit(x, y);
        rowHit = row;
        _editRow = row;
        break;
      }
      else if (row->rect->_shape.getGlobalBounds().contains(mousePos))
      {
        row->flags.Toggle(EffectRow::RowFlagsF::Selected);
        rowHit = row;
        break;
      }
    }

    if (rowHit)
    {
      // if a new row is hit, end any current editing
      if (_editRow && rowHit != _editRow)
      {
        _editRow->EndEditVars(true);
      }

      RecalcEffecRows();
    }
    else
    {
      _editRow = nullptr;
    }
  }
  else
  {
    for (EffectRow* row : effects)
    {
      if (row->KeyframeIntersect(mousePos))
      {
        _movingKeyframe = row;
        break;
      }
    }
  }
  return true;
}

//----------------------------------------------------------------------------------
bool TimelineWindow::OnMouseMoved(const Event& event)
{
  const editor::protocol::Settings& settings = EDITOR.Settings();
  Vector2i posModule = PointToLocal<int>(event.mouseMove.x, event.mouseMove.y);
  Vector2i posTimeline(posModule.x - settings.effect_view_width(), posModule.y);

  time_duration curTime = PixelToTime(posModule.x);

  // timeline scrolling has priority
  if (sf::Mouse::isButtonPressed(sf::Mouse::Middle))
  {
    if (_lastDragPos.x == -1)
    {
      _lastDragPos = Vector2i(event.mouseMove.x, event.mouseMove.y);
      return true;
    }
    int delta = event.mouseMove.x - _lastDragPos.x;
    time_duration d = AbsPixelToTime(delta);
    if (_panelOffset + d < seconds(0))
      _panelOffset = seconds(0);
    else 
    _panelOffset += AbsPixelToTime(delta);

    _lastDragPos = Vector2i(event.mouseMove.x, event.mouseMove.y);
    return true;
  }
  else if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && _movingKeyframe)
  {
    _movingKeyframe->UpdateKeyframe(curTime);
  }

  return true;
}

//----------------------------------------------------------------------------------
bool TimelineWindow::OnMouseButtonReleased(const Event& event)
{
  if (_movingKeyframe)
  {
    _movingKeyframe->EndKeyframeUpdate(false);
    _movingKeyframe = nullptr;
  }
  return true;
}

//----------------------------------------------------------------------------------
bool TimelineWindow::OnMouseWheelMoved(const Event& event)
{
  const editor::protocol::Settings& settings = EDITOR.Settings();

  if (event.mouseWheel.delta < 0)
    _pixelsPerSecond *= 2;
  else if (event.mouseWheel.delta > 0)
    _pixelsPerSecond /= 2;

  _pixelsPerSecond = Clamp(settings.timeline_zoom_min(), settings.timeline_zoom_max(), _pixelsPerSecond);

  return true;
}

//----------------------------------------------------------------------------------
void TimelineWindow::DrawTimeline()
{
//  _timelineTexture.clear();

  const editor::protocol::Settings& settings = EDITOR.Settings();

  Color rowCol = FromProtocol(settings.default_row_color());
  time_duration curTime = EDITOR.CurTime();

  // draw the ticker
  _texture.draw(_tickerRect->_shape);

  VertexArray lines(sf::Lines);
  int x = settings.effect_view_width();
  int y = settings.ticker_height() - 25;
  int minorInc = settings.ticker_interval() / settings.ticks_per_interval();
  Text text;
  text.setFont(_font);
  text.setCharacterSize(16);

  // calc rounded starting time
  while (x < _size.x)
  {
    time_duration t = PixelToTime(x);

    // round to nearest 0.250s
    int ms = t.total_milliseconds() / 250 * 250;
    t = milliseconds(ms);
    int tmpX = x;
    x = TimeToPixel(t);

    text.setString(to_string("%.2f", t.total_milliseconds() / 1000.0f));
    text.setPosition(x, y-20);
    _texture.draw(text);

    lines.append(sf::Vertex(Vector2f(x, y + 0)));
    lines.append(sf::Vertex(Vector2f(x, y + 20)));
    for (int i = 0; i < (int)settings.ticks_per_interval(); ++i)
    {
      lines.append(sf::Vertex(Vector2f(x + i * minorInc, y + 5)));
      lines.append(sf::Vertex(Vector2f(x + i * minorInc, y + 15)));
    }
    x = tmpX + settings.ticker_interval();
  }
  _texture.draw(lines);

  // draw time line
  VertexArray curLine(sf::Lines);
  int w = _size.x;
  y = settings.ticker_height() - 25;
  while (TimeToPixel(curTime - _panelOffset) > w)
  {
    _panelOffset += PixelToTime(w);
  }

  x = settings.effect_view_width();
  curLine.append(sf::Vertex(Vector2f(x, y), Color::White));
  curLine.append(sf::Vertex(Vector2f(x, _size.y), Color::White));

  x = TimeToPixel(EDITOR.CurTime());
  curLine.append(sf::Vertex(Vector2f(x, y), Color::Red));
  curLine.append(sf::Vertex(Vector2f(x, _size.y), Color::Red));
  _texture.draw(curLine);
}

//----------------------------------------------------------------------------------
void TimelineWindow::DrawEffects()
{
  // draw the header
  time_duration t = EDITOR.CurTime();
  Text text(to_string("TIME: %.3f, OFS: %.3f, SCALE: %.3f",
    t.total_milliseconds() / 1000.0f,
    _panelOffset.total_milliseconds() / 1000.0f,
    _pixelsPerSecond / 100.0f), _font);
  text.setPosition(10, 0);
  text.setCharacterSize(14);
  _texture.draw(text);

  for (EffectRow* row : _effectRows)
  {
    row->Draw(_texture);
  }
}

//----------------------------------------------------------------------------------
int TimelineWindow::TimeToPixel(const time_duration& t)
{
  int w = EDITOR.Settings().effect_view_width();

  // p = s * (t + a) + m
  // t = (p - m) / s - a
  double s = (double)_pixelsPerSecond / 1000.0;
  return w + s * (t.total_milliseconds() - _panelOffset.total_milliseconds());
}

//----------------------------------------------------------------------------------
time_duration TimelineWindow::PixelToTime(int x)
{
  double s = (double)_pixelsPerSecond / 1000.0;

  x -= EDITOR.Settings().effect_view_width();
  if (x < 0)
    return milliseconds(0);
    //    return time_duration(boost::posix_time::not_a_date_time);

  return milliseconds(x / s) + _panelOffset;
}

//----------------------------------------------------------------------------------
time_duration TimelineWindow::AbsPixelToTime(int x)
{
  return milliseconds(1000 * x / (int)_pixelsPerSecond);
}

//----------------------------------------------------------------------------------
void TimelineWindow::Draw()
{
  _texture.clear();

  DrawEffects();
  DrawTimeline();

  _texture.display();
}

//----------------------------------------------------------------------------------
TimelineWindow::EffectRow::EffectRow(
    const Font& font,
    const string& str,
    EffectRow* parent)
    : str(str)
    , parent(parent)
    , level(0)
{
  rect = STYLE_FACTORY.CreateStyledRectangle("default_row_color");
  keyframeRect = STYLE_FACTORY.CreateStyledRectangle("keyframe_style");
  keyframeRect->_shape.setSize(Vector2f(6, 6));

  text.setFont(font);
  text.setCharacterSize(16);
  flags.Set(EffectRow::RowFlagsF::Expanded);

  if (parent)
    level = parent->level + 1;
}

//----------------------------------------------------------------------------------
void TimelineWindow::EffectRow::Flatten(EffectRow* cur, vector<TimelineWindow::EffectRow*>* res)
{
  deque<EffectRow*> q({cur});
  while (!q.empty())
  {
    EffectRow* cur = q.front();
    q.pop_front();

    res->push_back(cur);
    for (EffectRow* c : cur->children)
      q.push_back(c);
  }
}

//----------------------------------------------------------------------------------
void TimelineWindow::EffectRow::Reposition(EffectRow* cur, float curY, float rowHeight)
{
  const editor::protocol::Settings& settings = EDITOR.Settings();
  float width = TimelineWindow::_instance->_size.x; // settings.effect_view_width();

  deque<EffectRow*> q({ cur });
  while (!q.empty())
  {
    EffectRow* cur = q.front();
    q.pop_front();

    cur->rect->_shape.setPosition(0, curY);
    cur->rect->_shape.setSize(Vector2f(width, (1 + cur->NumVars()) * rowHeight));
    curY += rowHeight;
    if (cur->flags.IsSet(EffectRow::RowFlagsF::Expanded))
    {
      for (EffectRow* c : cur->children)
        q.push_back(c);
    }

    const RectangleShape& shape = cur->rect->_shape;

    cur->expandRect = FloatRect(
        cur->level * 15 + shape.getPosition().x,
        shape.getPosition().y,
        15,
        15);

    u32 numVars = cur->NumVars();
    if (numVars > 0)
    {
      float x = cur->level * 15 + shape.getPosition().x;
      cur->varEditRect = FloatRect(
          x,
          shape.getPosition().y + 1 * rowHeight,
          width - x,
          numVars * rowHeight);
    }
  }
}

//----------------------------------------------------------------------------------
float TimelineWindow::EffectRow::RowHeight(EffectRow* cur, float rowHeight)
{
  float res = 0;
  deque<EffectRow*> q({ cur });
  while (!q.empty())
  {
    EffectRow* cur = q.front();
    q.pop_front();

    if (cur->flags.IsSet(EffectRow::RowFlagsF::Expanded))
    {
      res += (1 + cur->NumVars()) * rowHeight;
      for (EffectRow* c : cur->children)
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
TimelineWindow::EffectRowNoise::EffectRowNoise(
    const Font& font,
    const string& str,
    EffectRow* parent)
  : EffectRow(font, str, parent)
  , editingIdx(-1)
  , selectedKeyframe(nullptr)
{
}

//----------------------------------------------------------------------------------
void TimelineWindow::EffectRowNoise::DrawVars(RenderTexture& texture)
{
  const editor::protocol::Settings& settings = EDITOR.Settings();
  float h = settings.effect_row_height();

  Vector3f v = Interpolate(effector.displacement, EDITOR.CurTime().total_milliseconds());

  const Style* editingStyle = STYLE_FACTORY.GetStyle("var_text_editing");
  const Style* normalStyle = STYLE_FACTORY.GetStyle("var_text_normal");

  static const char* suffix[] = {
      "x: ",
      "y: ",
      "z: "
  };

  const auto& fnStyle = [this, editingStyle, normalStyle, v](int curIdx){
    if (curIdx == editingIdx)
    {
      text.setString(to_string("%s%s", suffix[curIdx], curEdit.c_str()).c_str());
      text.setColor(editingStyle->fillColor);
      text.setStyle(editingStyle->fontStyle);
    }
    else
    {
      text.setString(to_string("%s%.2f", suffix[curIdx], *(&v.x + curIdx)).c_str());
      text.setColor(normalStyle->fillColor);
      text.setStyle(normalStyle->fontStyle);
    }
  };

  fnStyle(0);
  text.setPosition(rect->_shape.getPosition() + Vector2f(20 + level * 15, h*1));
  texture.draw(text);

  fnStyle(1);
  text.setPosition(rect->_shape.getPosition() + Vector2f(20 + level * 15, h*2));
  texture.draw(text);

  fnStyle(2);
  text.setPosition(rect->_shape.getPosition() + Vector2f(20 + level * 15, h*3));
  texture.draw(text);

  // draw the keyframes
  VertexArray curLine(sf::Lines);
  Vector2f size = TimelineWindow::_instance->_size;
  int w = size.x - settings.effect_view_width();
  int x = settings.effect_view_width();
  float shapeY = rect->_shape.getPosition().y;

  for (int i = 0; i < 3; ++i)
  {
    int y = shapeY + h * (i+1.5f);
    curLine.append(sf::Vertex(Vector2f(x, y), Color::White));
    curLine.append(sf::Vertex(Vector2f(x + w, y), Color::White));
  }
  texture.draw(curLine);

  for (u32 i = 0; i < effector.displacement.keyframes.size(); ++i)
  {
    const Vector3Keyframe& keyframe = effector.displacement.keyframes[i];
    int x = TimelineWindow::_instance->TimeToPixel(milliseconds(keyframe.time));
    if (x < size.x)
    {
      // draw for each var
      for (u32 j = 0; j < 3; ++j)
      {
        int y = shapeY + h * (j+1.5f) - 3;
        keyframeRect->_shape.setPosition(x - 3, y);
        texture.draw(keyframeRect->_shape);
      }
    }
  }

  // Reset style
  fnStyle(-2);
}

//----------------------------------------------------------------------------------
float TimelineWindow::EffectRowNoise::NumVars()
{
  return 3;
}

//----------------------------------------------------------------------------------
void TimelineWindow::EffectRowNoise::InitVarEdit(float x, float y)
{
  const editor::protocol::Settings& settings = EDITOR.Settings();
  int h = settings.effect_row_height();
  prevValue = effector.displacement.keyframes[0].value;

  editingIdx = (y - rect->_shape.getPosition().y - h) / h;
  assert(editingIdx >= 0 && editingIdx <= 2);

  Vector3f v = Interpolate(effector.displacement, EDITOR.CurTime());
  switch (editingIdx)
  {
    case 0: curEdit = to_string("%.2f", v.x); break;
    case 1: curEdit = to_string("%.2f", v.y); break;
    case 2: curEdit = to_string("%.2f", v.z); break;
  }
}

//----------------------------------------------------------------------------------
void TimelineWindow::EffectRowNoise::EndEditVars(bool cancel)
{
  if (!cancel)
  {
    // create a new keyframe at the current position (if one doesn't exist)
    Vector3f v = prevValue;
    switch (editingIdx)
    {
      case 0: v.x = (float)atof(curEdit.c_str()); break;
      case 1: v.y = (float)atof(curEdit.c_str()); break;
      case 2: v.z = (float)atof(curEdit.c_str()); break;
    }

    AddKeyframe(EDITOR.CurTime(), v, &effector.displacement);
  }

  editingIdx = -1;
}

//----------------------------------------------------------------------------------
void TimelineWindow::EffectRowNoise::UpdateEditVar(Keyboard::Key key)
{
  if (key >= Keyboard::Key::Num0 && key <= Keyboard::Key::Num9)
  {
    curEdit += '0' + key - Keyboard::Key::Num0;
  }
  else if (key == Keyboard::Key::BackSpace)
  {
    if (curEdit.size() > 0)
    {
      curEdit.pop_back();
    }
  }
  else if (key == Keyboard::Key::Period && curEdit.find('.') == curEdit.npos)
  {
    curEdit += '.';
  }
}

//----------------------------------------------------------------------------------
bool TimelineWindow::EffectRowNoise::KeyframeIntersect(const Vector2f& pt)
{
  const editor::protocol::Settings& settings = EDITOR.Settings();

  Vector2f size = TimelineWindow::_instance->_size;
  int w = size.x - settings.effect_view_width();
  int x = settings.effect_view_width();
  float shapeY = rect->_shape.getPosition().y;
  float h = settings.effect_row_height();

  for (u32 i = 0; i < effector.displacement.keyframes.size(); ++i)
  {
    Vector3Keyframe& keyframe = effector.displacement.keyframes[i];
    int x = TimelineWindow::_instance->TimeToPixel(milliseconds(keyframe.time));
    if (x < size.x)
    {
      // draw for each var
      for (u32 j = 0; j < 3; ++j)
      {
        int y = shapeY + h * (j+1.5f) - 3;
        FloatRect rect(x, y, 6, 6);
        if (rect.contains(pt))
        {
          selectedKeyframe = &keyframe;
          return true;
        }
      }
    }
  }
}

//----------------------------------------------------------------------------------
void TimelineWindow::EffectRowNoise::UpdateKeyframe(const time_duration &t)
{
  if (!selectedKeyframe)
    return;

  selectedKeyframe->time = t.total_milliseconds();
}

//----------------------------------------------------------------------------------
void TimelineWindow::EffectRowNoise::EndKeyframeUpdate(bool cancel)
{
  selectedKeyframe = nullptr;
}


//----------------------------------------------------------------------------------
void TimelineWindow::EffectRow::Draw(RenderTexture& texture)
{
  const editor::protocol::Settings& settings = EDITOR.Settings();
  Color rowCol = FromProtocol(flags.IsSet(EffectRow::RowFlagsF::Selected)
      ? settings.selected_row_color()
      : settings.default_row_color());

  // draw background
  rect->_shape.setFillColor(rowCol);
  texture.draw(rect->_shape);

  // draw text
  text.setString(str);
  text.setPosition(rect->_shape.getPosition() + Vector2f(20 + level * 15, 0) );
  texture.draw(text);

  // draw expanded indicator
  VertexArray tri(sf::Triangles);
  float left = level * 15 + rect->_shape.getPosition().x;
  float top = rect->_shape.getPosition().y;
  float y = top;

  if (!flags.IsSet(EffectRow::RowFlagsF::Expanded))
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

    DrawVars(texture);

    for (EffectRow* child : children)
    {
      child->Draw(texture);
    }
  }
}

