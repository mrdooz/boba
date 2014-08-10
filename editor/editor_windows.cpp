#include "editor_windows.hpp"
#include "editor.hpp"

#include "protocol/effects_proto.hpp"
#include "effect_row.hpp"

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
    , _editRow(nullptr)
    , _tickerRect(nullptr)
    , _statusBar(nullptr)
    , _movingKeyframe(nullptr)
    , _selectedKeyframe(nullptr)
    , _displayMode(DisplayMode::Keyframe)
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

  int rowHeight = settings.effect_row_height();
  int curY = settings.ticker_height();

  const Vector2f& windowSize = GetSize();

  _tickerRect = STYLE_FACTORY.CreateStyledRectangle("default_row_color");
  _tickerRect->_shape.setPosition(width, 0);
  _tickerRect->_shape.setSize(Vector2f(_size.x - width, settings.ticker_height()));

  _statusBar = STYLE_FACTORY.CreateStyledRectangle("status_bar_style");
  _statusBar->_shape.setPosition(0, windowSize.y - settings.status_bar_height());
  _statusBar->_shape.setSize(Vector2f(windowSize.x, settings.status_bar_height()));

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
      parent->_children.push_back(new EffectRow(_font, str, parent));
      curY += rowHeight;
    }

    for (const NoiseEffector& e : p.noiseEffectors)
    {
      string str = to_string("Noise (%s)",
        e.applyTo == NoiseEffector::ApplyTo::Position ? "POS" : "SCALE");
      EffectRowNoise* n = new EffectRowNoise(_font, str, parent);
      n->_effector = e;
      parent->_children.push_back(n);
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
  Keyboard::Key code = event.key.code;

  // escape aborts the pending operation
  if (code == Keyboard::Escape)
  {
    if (_editRow)
    {
      _editRow->EndEditVars(false);
    }
    else if (_movingKeyframe)
    {
      _movingKeyframe->EndKeyframeUpdate(false);
    }

    _selectedKeyframe = nullptr;
    _movingKeyframe = nullptr;
  }
  else
  {
    if (_editRow)
    {
      if (code == Keyboard::Return)
      {
        _editRow->EndEditVars(true);
      }
      else
      {
        _editRow->UpdateEditVar(event.key.code);
      }
    }
    else
    {
      if (code == Keyboard::U && !_selectedRows.IsEmpty())
      {
        _displayMode = _selectedRows.Next() ? DisplayMode::Graph : DisplayMode::Keyframe;
      }
      else if (code == Keyboard::R)
      {
        _displayMode = DisplayMode::Keyframe;
        for (EffectRow* r : _selectedRows.backingSet)
        {
          r->ToggleGraphView(false);
        }
      }
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
    // Check for hits in the effect view
    EffectRow* rowHit = nullptr;
    for (EffectRow* row : effects)
    {
      if (row->_expandRect.contains(mousePos))
      {
        row->_flags.Toggle(EffectRow::RowFlagsF::Expanded);
        rowHit = row;
        break;
      }
      else if (row->_varEditRect.contains(mousePos))
      {
        row->BeginEditVars(x, y);
        rowHit = row;
        _editRow = row;
        break;
      }
      else if (row->_rect->_shape.getGlobalBounds().contains(mousePos))
      {
        _selectedRows.Toggle(row);
        row->_flags.Toggle(EffectRow::RowFlagsF::Selected);
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
    if (_displayMode == DisplayMode::Graph)
    {
      _selectedRows.CurRow()->GraphMouseButtonPressed(event);
    }
    else
    {
      if (_selectedKeyframe)
      {
        _selectedKeyframe->DeselectKeyframe();
        _selectedKeyframe = nullptr;
      }

      for (EffectRow* row : effects)
      {
        if (row->KeyframeIntersect(mousePos, _size))
        {
          _selectedKeyframe = row;
          break;
        }
      }
    }
  }
  return true;
}

//----------------------------------------------------------------------------------
bool TimelineWindow::OnMouseMoved(const Event& event)
{
  Vector2i posModule = PointToLocal<int>(event.mouseMove.x, event.mouseMove.y);

  time_duration curTime = PixelToTime(posModule.x);

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
  else if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
  {
    if (_displayMode == DisplayMode::Keyframe && _selectedKeyframe)
    {
      if (!_movingKeyframe)
      {
        // send the BeginKeyframeUpdate, and check if we're copying or moving
        _selectedKeyframe->BeginKeyframeUpdate(Keyboard::isKeyPressed(Keyboard::Key::LShift));
        _movingKeyframe = _selectedKeyframe;
      }

      _movingKeyframe->UpdateKeyframe(curTime);
    }
    else if (_displayMode == DisplayMode::Graph && _selectedRows.CurRow())
    {
      _selectedRows.CurRow()->GraphMouseMoved(event);
    }
  }

  return true;
}

//----------------------------------------------------------------------------------
bool TimelineWindow::OnMouseButtonReleased(const Event& event)
{
  if (_displayMode == DisplayMode::Graph)
  {
    if (_selectedRows.CurRow())
      return _selectedRows.CurRow()->GraphMouseButtonReleased(event);
  }
  else
  {
    if (_movingKeyframe)
    {
      _movingKeyframe->EndKeyframeUpdate(true);
      _movingKeyframe = nullptr;
    }
    return true;
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
  const editor::protocol::Settings& settings = EDITOR.Settings();

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
    row->Draw(_texture, _displayMode == DisplayMode::Keyframe);
  }

  if (_displayMode == DisplayMode::Graph && !_selectedRows.IsEmpty())
  {
    _selectedRows.CurRow()->DrawGraph(_texture, _size);
  }
}

//----------------------------------------------------------------------------------
void TimelineWindow::DrawStatusBar()
{
  const editor::protocol::Settings& settings = EDITOR.Settings();

  // draw the background
  _texture.draw(_statusBar->_shape);

  Text t;
  t.setFont(_font);
  t.setCharacterSize(16);

  // draw the status texts
  float x = 10;
  float y = GetPosition().y - settings.status_bar_height();
  for (const string& str : _statusBarValues)
  {
    t.setPosition(x, y);
    t.setString(str);
    _texture.draw(t);
  }

}

//----------------------------------------------------------------------------------
int TimelineWindow::TimeToPixel(const time_duration& t) const
{
  int w = EDITOR.Settings().effect_view_width();

  // p = s * (t + a) + m
  // t = (p - m) / s - a
  double s = (double)_pixelsPerSecond / 1000.0;
  return (int)(w + s * (t.total_milliseconds() - _panelOffset.total_milliseconds()));
}

//----------------------------------------------------------------------------------
time_duration TimelineWindow::PixelToTime(int x) const
{
  double s = (double)_pixelsPerSecond / 1000.0;

  x -= EDITOR.Settings().effect_view_width();
  if (x < 0)
    return milliseconds(0);

  return milliseconds(x / s) + _panelOffset;
}

//----------------------------------------------------------------------------------
time_duration TimelineWindow::AbsPixelToTime(int x) const
{
  return milliseconds(1000 * x / (int)_pixelsPerSecond);
}

//----------------------------------------------------------------------------------
void TimelineWindow::Draw()
{
  _texture.clear();

  DrawEffects();
  DrawTimeline();
  DrawStatusBar();

  _texture.display();
}

//----------------------------------------------------------------------------------
void TimelineWindow::UpdateStatusBar(int segment, const string& value)
{
  if (segment >= _statusBarValues.size())
    _statusBarValues.resize(segment + 1);

  _statusBarValues[segment] = value;
}


