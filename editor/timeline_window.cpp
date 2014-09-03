#include "timeline_window.hpp"
#include "editor.hpp"

#include "protocol/effects_proto.hpp"
#include "effect_row.hpp"
#include "row_var.hpp"

using namespace editor;
using namespace bristol;

#pragma warning(disable: 4244)

//----------------------------------------------------------------------------------
Vector2f Normalize(const Vector2f& v)
{
  float len = sqrtf(v.x*v.x + v.y*v.y);
  return v / len;
}

TimelineWindow* TimelineWindow::_instance;

//----------------------------------------------------------------------------------
TimelineWindow::TimelineWindow(
    const string& title,
    const Vector2f& pos,
    const Vector2f& size)
    : VirtualWindow(title, pos, size, bristol::WindowFlags(bristol::WindowFlag::StaticWindow))
    , _panelOffset(seconds(0))
    , _lastDragPos(-1, -1)
    , _tickerRect(nullptr)
    , _statusBar(nullptr)
    , _displayMode(DisplayMode::Keyframe)
    , _selectedVar(nullptr)
    , _tickIntervals({milliseconds(50), milliseconds(100), milliseconds(250), milliseconds(500), seconds(1), seconds(5), seconds(10), seconds(30), seconds(60)})
    , _curZoom(3)
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

  const Vector2f& windowSize = GetSize();

  _tickerRect._style = STYLE_FACTORY.CreateStyle("default_row_color");
  _tickerRect._rect.setPosition(width, 0);
  _tickerRect._rect.setSize(Vector2f(_size.x - width, settings.ticker_height()));

  _statusBar = STYLE_FACTORY.CreateStyle("status_bar_style");
  _statusBar._rect.setPosition(0, windowSize.y - settings.status_bar_height());
  _statusBar._rect.setSize(Vector2f(windowSize.x, settings.status_bar_height()));

  // add temporary effect rows until we get the websocket update
  AddDefaultRows(false);

  RecalcEffecRows();

  return true;
}

//----------------------------------------------------------------------------------
void TimelineWindow::AddDefaultRows(bool addKeyframes)
{
  string str;
  for (int i = 0; i < 5; ++i)
  {
    EffectRow* parent = new EffectRowPlexus(_font, "PLEXUS");
    _effectRows.push_back(parent);

    str = to_string("TextPath: %s", "test");
    parent->_children.push_back(new EffectRowTextPath(_font, str, parent));

    NoiseEffector e;
    e.applyTo = NoiseEffector::ApplyTo::Position;

    str = to_string("Noise (%s)", e.applyTo == NoiseEffector::ApplyTo::Position ? "POS" : "SCALE");
    EffectRowNoise* n = new EffectRowNoise(_font, str, parent);

    if (addKeyframes)
    {
      s64 t = randf(0, 500);

      // first pass to add the keyframes
      for (u32 j = 0; j < 50; ++j)
      {
        e.displacement.x.keyframe.push_back({ t, randf(-10.f, 20.f) });
        t += randf(150, 1000);
        e.displacement.y.keyframe.push_back({ t, randf(-10.f, 20.f) });
        t += randf(150, 1000);
        e.displacement.z.keyframe.push_back({ t, randf(-10.f, 20.f) });
        t += randf(150, 1000);
      }

      // second pass to set up in/out keys
      for (u32 j = 0; j < 50; ++j)
      {
        float s = 10;
        if (j == 0)
        {
          const auto& fn = [&](FloatAnim* f){
              float v = (f->keyframe[1].key.value - f->keyframe[0].key.value) / (f->keyframe[1].key.time - f->keyframe[0].key.time);

              f->keyframe[j].cpOut = f->keyframe[0].key;
              f->keyframe[j].cpOut.time += 200;
              f->keyframe[j].cpOut.value += s * v;
          };

          fn(&e.displacement.x);
          fn(&e.displacement.y);
          fn(&e.displacement.z);
        }
        else if (j == 49)
        {
          const auto& fn = [&](FloatAnim* f){
              float v = (f->keyframe[49].key.value - f->keyframe[48].key.value) / (f->keyframe[49].key.time - f->keyframe[48].key.time);

              f->keyframe[j].cpIn = f->keyframe[49].key;
              f->keyframe[j].cpIn.time -= 200;
              f->keyframe[j].cpIn.value -= s * v;
          };

          fn(&e.displacement.x);
          fn(&e.displacement.y);
          fn(&e.displacement.z);
        }
        else
        {
          const auto& fn = [&](FloatAnim* f) {
              float v = (f->keyframe[j+1].key.value - f->keyframe[j-1].key.value) / (f->keyframe[j+1].key.time - f->keyframe[j-1].key.time);

              f->keyframe[j].cpIn = f->keyframe[j].key;
              f->keyframe[j].cpOut = f->keyframe[j].key;

              f->keyframe[j].cpIn.time -= 200;
              f->keyframe[j].cpIn.value -= s * v;

              f->keyframe[j].cpOut.time += 200;
              f->keyframe[j].cpOut.value += s * v;
          };

          fn(&e.displacement.x);
          fn(&e.displacement.y);
          fn(&e.displacement.z);
        }
      }
    }

    n->_effector = e;
    parent->_children.push_back(n);
  }

}


//----------------------------------------------------------------------------------
void TimelineWindow::RecalcEffecRows()
{
  const editor::protocol::Settings& settings = EDITOR.Settings();
  float rowHeight = settings.effect_row_height();

  float curY = settings.ticker_height();

  for (EffectRow* row : _effectRows)
  {
    row->Reposition(curY, rowHeight);
    curY += row->RowHeight(rowHeight);
  }
}

//----------------------------------------------------------------------------------
bool TimelineWindow::OnKeyReleased(const Event &event)
{

  vector<EffectRow*> effects;
  for (EffectRow* row : _effectRows)
    row->Flatten(&effects);

  // check for hits on an effect row
  for (EffectRow* row : effects)
  {
    if (row->OnKeyReleased(event))
    {
      return true;
    }
  }


  // The current row didn't handle the key..
  Keyboard::Key code = event.key.code;

  switch (code)
  {
    case Keyboard::Home:
      EDITOR.SetCurTime(seconds(0));
      break;

    case Keyboard::U:
      _displayMode = NextEnum(_displayMode);
      break;

    case Keyboard::R:
      _displayMode = DisplayMode::Keyframe;
      break;
  }



  return true;
}

//----------------------------------------------------------------------------------
bool TimelineWindow::OnMouseButtonPressed(const Event& event)
{
  const editor::protocol::Settings& settings = EDITOR.Settings();

  int y = (int)(event.mouseButton.y - _pos.y);

  // check for a click in the ticker
  if (y < (int)settings.ticker_height())
  {
    time_duration t = PixelToTime(event.mouseButton.x - (int)_pos.x);
    if (!t.is_not_a_date_time())
    {
      EDITOR.SetCurTime(t);
    }
    return true;
  }

  vector<EffectRow*> effects;
  for (EffectRow* row : _effectRows)
    row->Flatten(&effects);

  // check for hits on an effect row
  for (EffectRow* row : effects)
  {
    if (row->OnMouseButtonPressed(event))
    {
      RecalcEffecRows();
      return true;
    }
  }

  return false;
}

//----------------------------------------------------------------------------------
bool TimelineWindow::OnMouseMoved(const Event& event)
{
//  Vector2i posModule = PointToLocal<int>(event.mouseMove.x, event.mouseMove.y);

  // Check if the timeline is being dragged
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

  vector<EffectRow*> effects;
  for (EffectRow* row : _effectRows)
    row->Flatten(&effects);

  for (EffectRow* row : effects)
  {
    if (row->OnMouseMoved(event))
    {
      return true;
    }
  }

  return false;
}

//----------------------------------------------------------------------------------
bool TimelineWindow::OnMouseButtonReleased(const Event& event)
{
  _lastDragPos = {-1, -1};

  vector<EffectRow*> effects;
  for (EffectRow* row : _effectRows)
    row->Flatten(&effects);

  // check for hits on an effect row
  for (EffectRow* row : effects)
  {
    if (row->OnMouseButtonReleased(event))
    {
      RecalcEffecRows();
      return true;
    }
  }

  return false;
}

//----------------------------------------------------------------------------------
bool TimelineWindow::OnMouseWheelMoved(const Event& event)
{
  // Note, due to the mouse wheel implementation in SFML, delta will often be 0 for small
  // movements
  _curZoom = Clamp(0, (int)_tickIntervals.size() -1,
      _curZoom + (event.mouseWheel.delta < 0 ? -1 : event.mouseWheel.delta > 1 ? 1 : 0));

  //printf("wheel: %d %d %d\n", event.mouseWheel.delta, event.mouseWheel.x, event.mouseWheel.y);

  return true;
}

//----------------------------------------------------------------------------------
void TimelineWindow::DrawTimeline()
{
  const editor::protocol::Settings& settings = EDITOR.Settings();

  float w = settings.effect_view_width();

  RectangleShape s;
  s.setPosition(w, 0);
  s.setSize(Vector2f(_size.x - w, _size.y));
  s.setFillColor(::FromProtocol(settings.timeline_view_background_color()));
  _texture.draw(s);


  // If running, adjust the panel offset so it contains the timeline
  if (!EDITOR._stateFlags.IsSet(Editor::StateFlagsF::Paused))
  {
    time_duration curTime = EDITOR.CurTime();
    int pageWidth = GetSize().x - w;
    time_duration pageTime = AbsPixelToTime(pageWidth);

    while (_panelOffset + pageTime < curTime)
    {
      _panelOffset += pageTime;
    }
  }

  // draw the ticker
  _tickerRect.Apply();
  _texture.draw(_tickerRect._rect);

  VertexArray lines(sf::Lines);
  int y = settings.ticker_height() - 25;
  int minorInc = settings.ticker_interval() / settings.ticks_per_interval();
  Text text;
  text.setFont(_font);
  text.setCharacterSize(16);

  // round to nearest tick interval
  s64 tick = _tickIntervals[_curZoom].total_milliseconds();
  s64 curTime = _panelOffset.total_milliseconds() / tick * tick;
  int x = TimeToPixel(curTime);
  while (x < _size.x)
  {
    text.setString(to_string("%.2f", curTime / 1000.0f).c_str());
    text.setPosition(x, y-20);
    _texture.draw(text);

    lines.append(sf::Vertex(Vector2f(x, y + 0)));
    lines.append(sf::Vertex(Vector2f(x, y + 20)));
    for (int i = 0; i < (int)settings.ticks_per_interval(); ++i)
    {
      lines.append(sf::Vertex(Vector2f(x + i * minorInc, y + 5)));
      lines.append(sf::Vertex(Vector2f(x + i * minorInc, y + 15)));
    }

    x += settings.ticker_interval();
    curTime += tick;
  }
  _texture.draw(lines);

}

//----------------------------------------------------------------------------------
void TimelineWindow::DrawTimelinePost()
{
  const editor::protocol::Settings& settings = EDITOR.Settings();

  VertexArray curLine(sf::Lines);

  // border line
  int x = settings.effect_view_width();
  int y = settings.ticker_height() - 25;
  curLine.append(sf::Vertex(Vector2f(x, y), Color::White));
  curLine.append(sf::Vertex(Vector2f(x, _size.y), Color::White));

  // time line
  x = TimeToPixel(EDITOR.CurTime());
  if (x >= (int)settings.effect_view_width())
  {
    LineStrip ll(2, Color::Red);
    ll.addPoint({(float)x, (float)y});
    ll.addPoint({(float)x, (float)_size.y});
    _texture.draw(ll);
  }

  _texture.draw(curLine);
}

//----------------------------------------------------------------------------------
void TimelineWindow::DrawEffects()
{
  const editor::protocol::Settings& settings = EDITOR.Settings();
  float w = settings.effect_view_width();

  RectangleShape s;
  s.setPosition(0, 0);
  s.setSize(Vector2f(w, _size.y));
  s.setFillColor(::FromProtocol(settings.effect_view_background_color()));
  _texture.draw(s);

  // draw the header
  time_duration t = EDITOR.CurTime();
  Text text(to_string("TIME: %.3f, OFS: %.3f, SCALE: %.3f",
      t.total_milliseconds() / 1000.0f,
      _panelOffset.total_milliseconds() / 1000.0f,
      (float)_tickIntervals[_curZoom].total_milliseconds()), _font);
  text.setPosition(10, 0);
  text.setCharacterSize(14);
  _texture.draw(text);

  if (_displayMode == DisplayMode::Graph && _selectedVar)
  {
    _selectedVar->DrawGraph(_texture);
  }

  for (EffectRow* row : _effectRows)
  {
    row->Draw(_texture, _displayMode == DisplayMode::Keyframe);
  }
}

//----------------------------------------------------------------------------------
void TimelineWindow::DrawStatusBar()
{
  const editor::protocol::Settings& settings = EDITOR.Settings();

  // draw the background
  _statusBar.Apply();
  _texture.draw(_statusBar._rect);

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
int TimelineWindow::TimeToPixel(s64 ms) const
{
  const editor::protocol::Settings& settings = EDITOR.Settings();
  int w = settings.effect_view_width();
  return w + (ms - _panelOffset.total_milliseconds()) / (float)_tickIntervals[_curZoom].total_milliseconds() * settings.ticker_interval();
}

//----------------------------------------------------------------------------------
int TimelineWindow::TimeToPixel(const time_duration& t) const
{
  return TimeToPixel(t.total_milliseconds());
}

//----------------------------------------------------------------------------------
time_duration TimelineWindow::PixelToTime(int x) const
{
  const editor::protocol::Settings& settings = EDITOR.Settings();
  int w = settings.effect_view_width();
  x -= w;
  if (x < 0)
    return milliseconds(0);

  return _tickIntervals[_curZoom] * (float)x / settings.ticker_interval() + _panelOffset;
}

//----------------------------------------------------------------------------------
s64 TimelineWindow::PixelToTimeMs(int x) const
{
  return PixelToTime(x).total_milliseconds();
}

//----------------------------------------------------------------------------------
time_duration TimelineWindow::AbsPixelToTime(int x) const
{
  const editor::protocol::Settings& settings = EDITOR.Settings();
  return _tickIntervals[_curZoom] * x / (float)settings.ticker_interval();
}

//----------------------------------------------------------------------------------
void TimelineWindow::DrawBackground()
{
  const editor::protocol::Settings& settings = EDITOR.Settings();
  float w = settings.effect_view_width();

  RectangleShape s;
  s.setPosition(w, 0);
  s.setSize(Vector2f(_size.x - w, _size.y));
  s.setFillColor(::FromProtocol(settings.timeline_view_background_color()));
  _texture.draw(s);
}

//----------------------------------------------------------------------------------
void TimelineWindow::Draw()
{
  _texture.clear();

  DrawBackground();
  DrawTimeline();
  DrawEffects();
  DrawTimelinePost();
  DrawStatusBar();

  _texture.display();
}

//----------------------------------------------------------------------------------
void TimelineWindow::Update()
{
}

//----------------------------------------------------------------------------------
void TimelineWindow::UpdateStatusBar(int segment, const string& value)
{
  if (segment >= _statusBarValues.size())
    _statusBarValues.resize(segment + 1);

  _statusBarValues[segment] = value;
}

//----------------------------------------------------------------------------------
void TimelineWindow::KeyframesModified()
{
  effect::protocol::EffectSettings settings;
  for (const EffectRow* row : _effectRows)
  {
    effect::protocol::EffectSetting* setting = settings.add_effect_setting();
    setting->set_id(row->_id);
    row->ToProtocol(setting);
  }

  EDITOR.SettingsUpdated(settings);
}

//----------------------------------------------------------------------------------
void TimelineWindow::Reset(const effect::protocol::EffectSettings& settings)
{
  for (EffectRow* e : _effectRows)
    delete e;
  _effectRows.clear();

  for (const effect::protocol::EffectSetting& setting : settings.effect_setting())
  {
    switch (setting.type())
    {
      case 0:
        break;

      case 1:
        effect::protocol::plexus::Plexus p;
        const string& str = setting.config_msg();
        p.ParseFromArray(str.data(), (int)str.size());

        EffectRow* parent = new EffectRowPlexus(_font, "PLEXUS");
        parent->_id = setting.id();
        _effectRows.push_back(parent);
        parent->FromProtocol(p);

        break;
    }
  }

  RecalcEffecRows();
}

//----------------------------------------------------------------------------------
void TimelineWindow::SendEffectEvent(RowVar* sender, const EffectRowEvent& event)
{
  switch (event.type)
  {
    case EffectRowEvent::Type::VarSelected:
    {
      _selectedVar = sender;

      for (EffectRow* e : _effectRows)
        e->SendEvent(sender, event);
      break;
    }
  }
}
