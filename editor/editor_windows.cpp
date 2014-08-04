#include "editor_windows.hpp"
#include "editor.hpp"
#include "proto_utils.hpp"

#include "protocol/effects_proto.hpp"

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

//----------------------------------------------------------------------------------
TimelineWindow::TimelineWindow(
  const string& title,
  const Vector2f& pos,
  const Vector2f& size)
    : VirtualWindow(title, pos, size, bristol::WindowFlags(bristol::WindowFlag::StaticWindow))
    , _lastDragPos(-1, -1)
    , _panelOffset(seconds(0))
    , _pixelsPerSecond(EDITOR.Settings().timeline_zoom_default())
    , _selectedModule(nullptr)
    , _selectedEffect(nullptr)
{
}

//----------------------------------------------------------------------------------
TimelineWindow::~TimelineWindow()
{
  SeqDelete(&_rows);
  SeqDelete(&_modules);
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

  if (!_font.loadFromFile(EDITOR.GetAppRoot() + "gfx/coders_crux.ttf"))
    return false;

  const editor::Settings& settings = EDITOR.Settings();

  
  Vector2i moduleSize(settings.module_view_width(), settings.module_row_height());
  int y = settings.module_row_height() - 2;

  // create render textures and sprites
  _moduleTexture.create(settings.module_view_width(), _size.y);
  _moduleSprite.setTexture(_moduleTexture.getTexture(), true);

  _timelineTexture.create(_size.x - settings.module_view_width(), _size.y);
  _timelineSprite.setTexture(_timelineTexture.getTexture(), true);
  _timelineSprite.setPosition(settings.module_view_width(), 0);

  return true;
}

//----------------------------------------------------------------------------------
void TimelineWindow::ResetDragDrop()
{
  _draggingEffect.Reset();
  if (_selectedEffect)
  {
    _selectedEffect->flags.Clear(EffectInstanceFlagsF::Move);
    _selectedEffect->flags.Clear(EffectInstanceFlagsF::ResizeStart);
    _selectedEffect->flags.Clear(EffectInstanceFlagsF::ResizeEnd);
  }

  _draggingModule.Reset();
  if (_selectedModule)
  {
    // reset any module flags..
  }

  _lastDragPos = Vector2i(-1, -1);

  _timelineFlags.Clear(TimelineFlagsF::PendingDrag);
  _timelineFlags.Clear(TimelineFlagsF::PendingEffectMove);
}

//----------------------------------------------------------------------------------
bool TimelineWindow::OnMouseButtonPressed(const Event& event)
{
  ResetDragDrop();

  const editor::Settings& settings = EDITOR.Settings();

  int x = (int)(event.mouseButton.x - _pos.x);
  int y = (int)(event.mouseButton.y - _pos.y);
  if (x < (int)settings.module_view_width())
  {
    // module drag/drop
    for (Module* m : _modules)
    {
      m->flags.Clear(ModuleFlagsF::Selected);
    }

    // handle component click
    for (Module* m : _modules)
    {
      if (m->rect.contains(x, y))
      {
        m->flags.Set(ModuleFlagsF::Selected);
        _selectedModule = m;
        _timelineFlags.Set(TimelineFlagsF::PendingDrag);
        break;
      }
    }
  }
  else if (y < (int)settings.module_row_height())
  {
    time_duration t = PixelToTime(event.mouseButton.x - (int)_pos.x);
    if (!t.is_not_a_date_time())
    {
      EDITOR.SetCurTime(t);
    }
  }
  else
  {
    // effect movement
    for (Row* row : _rows)
    {
      for (EffectInstance* e : row->effects)
      {
        e->flags.Clear(EffectInstanceFlagsF::Selected);
      }

      if (!row->rect.contains(x, y))
        continue;

      time_duration t = PixelToTime(x);
      for (EffectInstance* e : row->effects)
      {
        if (t >= e->startTime && t < e->endTime)
        {
          _timelineFlags.Set(TimelineFlagsF::PendingEffectMove);
          e->flags.Set(EffectInstanceFlagsF::Selected);
          _selectedEffect = e;
        }
      }
    }

  }
  return true;
}

//----------------------------------------------------------------------------------
bool TimelineWindow::OnMouseMoved(const Event& event)
{
  const editor::Settings& settings = EDITOR.Settings();
  Vector2i posModule = PointToLocal<int>(event.mouseMove.x, event.mouseMove.y);
  Vector2i posTimeline(posModule.x - settings.module_view_width(), posModule.y);

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

  // should we start module drag/drop
  if (_timelineFlags.IsSet(TimelineFlagsF::PendingDrag))
  {
    _draggingModule.module = _selectedModule;
    _timelineFlags.Clear(TimelineFlagsF::PendingDrag);
  }

  // check if any module is being dragged
  if (_draggingModule.module)
  {
    // check if the module is above any row
    for (Row* row : _rows)
    {
      if (row->rect.contains(posModule))
      {
        // check if there is space for the current module
        _draggingModule.dragStart = curTime;
        _draggingModule.dragEnd   = curTime + seconds(5);
        row->TimeFixup(&_draggingModule.dragStart, &_draggingModule.dragEnd, nullptr);
        _draggingModule.dragPos.x = TimeToPixel(_draggingModule.dragStart);
        _draggingModule.dragPos.y = row->rect.top;

        break;
      }
    }
  }

  // check for pending effect move
  if (_timelineFlags.IsSet(TimelineFlagsF::PendingEffectMove))
  {
    _timelineFlags.Clear(TimelineFlagsF::PendingEffectMove);

    _draggingEffect.effect    = _selectedEffect;
    _draggingEffect.startPos  = posTimeline.x;
    _draggingEffect.orgStart  = _selectedEffect->startTime;
    _draggingEffect.orgEnd    = _selectedEffect->endTime;

    const Row* r        = _selectedEffect->row;
    IntRect leftRect    = r->rect;
    leftRect.width      = settings.resize_handle();
    leftRect.left       = TimeToPixel(_selectedEffect->startTime);

    IntRect rightRect   = leftRect;
    rightRect.left      = TimeToPixel(_selectedEffect->endTime) - settings.resize_handle();

    IntRect middleRect  = leftRect;
    middleRect.left     = leftRect.left + leftRect.width;
    middleRect.width    = rightRect.left - middleRect.left;

    // check if this is a move or a resize
    if (leftRect.contains(posTimeline))
    {
      _selectedEffect->flags.Set(EffectInstanceFlagsF::ResizeStart);
    }
    else if (rightRect.contains(posTimeline))
    {
      _selectedEffect->flags.Set(EffectInstanceFlagsF::ResizeEnd);
    }
    else if (middleRect.contains(posTimeline))
    {
      _selectedEffect->flags.Set(EffectInstanceFlagsF::Move);
    }
  }

  if (_draggingEffect.effect)
  {
    // calc delta, and apply
    time_duration delta = AbsPixelToTime(posTimeline.x - _draggingEffect.startPos);
    EffectInstance* e   = _draggingEffect.effect;

    EffectInstanceFlags f = _selectedEffect->flags;
    bool start  = f.IsSet(EffectInstanceFlagsF::Move) || f.IsSet(EffectInstanceFlagsF::ResizeStart);
    bool end    = f.IsSet(EffectInstanceFlagsF::Move) || f.IsSet(EffectInstanceFlagsF::ResizeEnd);

    const Row* row = e->row;
    time_duration tmpStart(_draggingEffect.orgStart + delta);
    time_duration tmpEnd(_draggingEffect.orgEnd + delta);
    time_duration newStart(tmpStart);
    time_duration newEnd(tmpEnd);

    if (start)
      newStart = row->SnappedPosition(tmpStart, false, e);

    if (end)
      newEnd = row->SnappedPosition(tmpEnd, true, e);

    // if updating both start and end, only update if neither one gets clamped
    if (start && tmpEnd == newEnd)
      e->startTime = newStart;

    if (end && tmpStart == newStart)
      e->endTime = newEnd;
  }
  return true;
}

//----------------------------------------------------------------------------------
bool TimelineWindow::OnMouseButtonReleased(const Event& event)
{
  if (_draggingModule.module)
  {
    // check if module dropped on any row
    Vector2i pos = PointToLocal<int>(event.mouseButton.x, event.mouseButton.y);
    for (Row* row : _rows)
    {
      if (row->rect.contains(pos))
      {
        row->AddEffect(new EffectInstance { _draggingModule.dragStart, _draggingModule.dragEnd, _draggingModule.module, row });
        break;
      }
    }
  }

  ResetDragDrop();
  return true;
}

//----------------------------------------------------------------------------------
bool TimelineWindow::OnMouseWheelMoved(const Event& event)
{
  const editor::Settings& settings = EDITOR.Settings();

  if (event.mouseWheel.delta < 0)
    _pixelsPerSecond *= 2;
  else if (event.mouseWheel.delta > 0)
    _pixelsPerSecond /= 2;

  _pixelsPerSecond = Clamp(settings.timeline_zoom_min(), settings.timeline_zoom_max(), _pixelsPerSecond);

  return true;
}

//----------------------------------------------------------------------------------
void TimelineWindow::DrawEffect(const EffectInstance& effect, const Row& row, Text& text)
{
  const editor::Settings& settings = EDITOR.Settings();
  Color hoverCol = FromProtocol(settings.hover_row_color());
  Color selectedCol = FromProtocol(settings.selected_row_color());

  int start = TimeToPixel(effect.startTime);
  int end = TimeToPixel(effect.endTime);

  int diff = (row.rect.height - settings.effect_height()) / 2;
  Vector2f pos(start, row.rect.top + diff);
  Vector2f size(end - start, settings.effect_height());
  DrawRectOutline(_timelineTexture, pos, size,
      effect.flags.IsSet(EffectInstanceFlagsF::Selected) ? selectedCol : hoverCol, 2, settings.resize_handle());

  text.setString(effect.module->name);
  Vector2f ofs(text.getLocalBounds().width, text.getLocalBounds().height);
  text.setPosition(pos + (size - ofs) / 2.0f);
  _timelineTexture.draw(text);

}

//----------------------------------------------------------------------------------
void TimelineWindow::DrawTimeline()
{
  _timelineTexture.clear();

  const editor::Settings& settings = EDITOR.Settings();

  Color rowCol = FromProtocol(settings.default_row_color());
  time_duration curTime = EDITOR.CurTime();

  // draw the ticker
  RectangleShape ticker;
  ticker.setFillColor(rowCol);
  ticker.setSize(Vector2f(_size.x, settings.ticker_height()));
  _timelineTexture.draw(ticker);

  VertexArray lines(sf::Lines);
  int x = 0;
  int y = settings.ticker_height() - 25;
  int minorInc = settings.ticker_interval() / settings.ticks_per_interval();
  Text text;
  text.setFont(_font);
  text.setCharacterSize(16);
  while (x < _size.x)
  {
    // need to cheese the 'x' value to make it relative the whole window, and not just the
    // timline part
    time_duration t = PixelToTime(x + settings.module_view_width());
    text.setString(to_string("%.1f", t.total_milliseconds() / 1000.0f));
    text.setPosition(x, y-20);
    _timelineTexture.draw(text);
    lines.append(sf::Vertex(Vector2f(x, y + 0)));
    lines.append(sf::Vertex(Vector2f(x, y + 20)));
    int tmpX = x;
    for (int i = 0; i < (int)settings.ticks_per_interval(); ++i)
    {
      lines.append(sf::Vertex(Vector2f(x + i * minorInc, y + 5)));
      lines.append(sf::Vertex(Vector2f(x + i * minorInc, y + 15)));
    }
    x = tmpX + settings.ticker_interval();
  }
  _timelineTexture.draw(lines);

  // draw the rows
  x = 0;
  y = settings.ticker_height();
  text.setCharacterSize(16);
  text.setFont(_font);
  for (const Row* row : _rows)
  {
    DrawRectOutline(_timelineTexture, Vector2f(row->rect.left, row->rect.top), 
        Vector2f(row->rect.width, row->rect.height), rowCol, 2);

    for (const EffectInstance* e : row->effects)
    {
      DrawEffect(*e, *row, text);
    }
  }

  // draw time line
  VertexArray curLine(sf::Lines);
  int w = _size.x;
  y = settings.ticker_height() - 25;
  while (TimeToPixel(curTime - _panelOffset) > w)
  {
    _panelOffset += PixelToTime(w);
  }

  x = TimeToPixel(EDITOR.CurTime());
  curLine.append(sf::Vertex(Vector2f(x, y), Color::Red));
  curLine.append(sf::Vertex(Vector2f(x, _size.y), Color::Red));
  _timelineTexture.draw(curLine);

  // draw the drag/drop module
  DrawDraggingModule();

  _timelineTexture.display();
  _texture.draw(_timelineSprite);
}

//----------------------------------------------------------------------------------
void TimelineWindow::DrawDraggingModule()
{
  if (!_draggingModule.module)
    return;

  const editor::Settings& settings = EDITOR.Settings();
  Color selectedRowCol = FromProtocol(settings.selected_row_color());
  Color invalidHover = FromProtocol(settings.invalid_hover_row_color());

  Vector2f pos(_draggingModule.dragPos.x, _draggingModule.dragPos.y);
  Color col = selectedRowCol;
  Vector2f size(TimeToPixel(_draggingModule.dragEnd) - pos.x, settings.module_row_height());

  DrawRectOutline(_timelineTexture, pos, size, col, 2);
  Text text;
  text.setString(_draggingModule.module->name);
  text.setCharacterSize(16);
  text.setFont(_font);
  Vector2f ofs(text.getLocalBounds().width, text.getLocalBounds().height);
  text.setPosition(pos + (size - ofs) / 2.0f);
  _timelineTexture.draw(text);
}

//----------------------------------------------------------------------------------
void TimelineWindow::DrawModule(float x, float y, const Module& module)
{
  const editor::Settings& settings = EDITOR.Settings();
  Color rowCol = FromProtocol(settings.default_row_color());
  Color selectedRowCol = FromProtocol(settings.selected_row_color());

  Vector2f size(module.rect.width, module.rect.height);
  Color col = module.flags.IsSet(ModuleFlagsF::Selected) ? selectedRowCol : rowCol;
  DrawRectOutline(_moduleTexture, Vector2f(x,y), size, col, 2);
  Text text;
  text.setString(module.name);
  text.setCharacterSize(16);
  text.setFont(_font);
  Vector2f ofs(text.getLocalBounds().width, text.getLocalBounds().height);
  text.setPosition(Vector2f(x, y) + (size - ofs) / 2.0f);
  _moduleTexture.draw(text);
}

struct RowDrawer
{
  RowDrawer(Text& text, int rowHeight, RenderTexture& texture, int x, int* y) 
    : text(text)
    , rowHeight(rowHeight)
    , texture(texture)
    , x(x)
    , y(y)
  {}

  void DrawText(const char* str)
  {
    text.setString(str);
    text.setPosition(10, *y);
    texture.draw(text);
    *y += rowHeight + 1;
  }

  Text& text;
  int rowHeight;
  RenderTexture& texture;
  int x;
  int* y;
};

//----------------------------------------------------------------------------------
void TimelineWindow::DrawPlexus(const Plexus& plexus)
{
  const editor::Settings& settings = EDITOR.Settings();
  Color rowCol = FromProtocol(settings.default_row_color());

  int fontSize = 16;
  int y = 100;

  Text text;
  text.setCharacterSize(fontSize);
  text.setFont(_font);

  RowDrawer row(text, fontSize + 1, _moduleTexture, 10, &y);
  row.DrawText("PLEXUS");

  // draw paths
  for (const TextPath& p : plexus.textPaths)
  {
    row.DrawText(to_string(">> TEXT PATH: %s", p.text.c_str()).c_str());
  }

  // draw effectors
  for (const NoiseEffector& p : plexus.noiseEffectors)
  {
    row.DrawText(to_string(">> NOISE EFFECTOR (%s)", p.applyTo == NoiseEffector::ApplyTo::Scale ? "SCALE" : "POS").c_str());

    // calc the interpolated values
    Vector3f v = Interpolate(p.displacement, EDITOR.CurTime().total_milliseconds());
    row.DrawText(to_string("    X: %.2f", v.x).c_str());
    row.DrawText(to_string("    Y: %.2f", v.y).c_str());
    row.DrawText(to_string("    Z: %.2f", v.z).c_str());
  }

}

//----------------------------------------------------------------------------------
void TimelineWindow::DrawEffects()
{
  _moduleTexture.clear();

  // draw the header
  time_duration t = EDITOR.CurTime();
  Text text(to_string("TIME: %.3f, OFS: %.3f, SCALE: %.3f",
    t.total_milliseconds() / 1000.0f,
    _panelOffset.total_milliseconds() / 1000.0f,
    _pixelsPerSecond / 100.0f), _font);
  text.setPosition(10, 0);
  text.setCharacterSize(14);
  _moduleTexture.draw(text);

  DrawPlexus(EDITOR._plexus);

  for (const Module* module : _modules)
  {
    DrawModule(module->rect.left, module->rect.top, *module);
  }
  _moduleTexture.display();
  _texture.draw(_moduleSprite);
}

//----------------------------------------------------------------------------------
int TimelineWindow::TimeToPixel(const time_duration& t)
{
  // p = s * (t + a) + m
  // t = (p - m) / s - a
  double s = (double)_pixelsPerSecond / 1000.0;
  return s * (t.total_milliseconds() - _panelOffset.total_milliseconds());
}

//----------------------------------------------------------------------------------
time_duration TimelineWindow::PixelToTime(int x)
{
  double s = (double)_pixelsPerSecond / 1000.0;

  x -= EDITOR.Settings().module_view_width();
  if (x < 0)
    return time_duration(boost::posix_time::not_a_date_time);

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
time_duration TimelineWindow::Row::SnappedPosition(
    const time_duration& t,
    bool start,
    EffectInstance* cur) const
{
  for (const EffectInstance* e : effects)
  {
    if (e == cur)
      continue;

    if (t >= e->startTime && t < e->endTime)
      return start ? e->startTime : e->endTime;
  }

  return t;
}

//----------------------------------------------------------------------------------
bool TimelineWindow::Row::IsValidPosition(
    const time_duration& t,
    EffectInstance* cur) const
{
  // check if the given time is inside any effect
  for (const EffectInstance* e : effects)
  {
    if (e == cur)
      continue;

    if (t >= e->startTime && t < e->endTime)
      return false;
  }
  return true;
}

//----------------------------------------------------------------------------------
void TimelineWindow::Row::TimeFixup(
    time_duration* start,
    time_duration* end,
    EffectInstance* cur) const
{
  time_duration delta = *end - *start;

  for (const EffectInstance* e : effects)
  {
    if (e == cur)
      continue;

    // check for overlap
    if (*start < e->startTime && *end >= e->startTime)
    {
      *end = e->startTime;
      return;
    }

    // if start overlaps an existing effect, bump it to after that effect
    // note, we need to keep going, because this bump might interfere with the
    // next effects
    if (*start >= e->startTime && *start <= e->endTime)
    {
      *start = e->endTime;
      *end = *start + delta;
    }
  }
}

//----------------------------------------------------------------------------------
void TimelineWindow::Row::AddEffect(EffectInstance* effect)
{
  effects.push_back(effect);

  // sort effect by start time
  sort(effects.begin(), effects.end(), [](const EffectInstance* lhs, const EffectInstance* rhs)
  {
    return lhs->startTime < rhs->startTime;
  });
}

//----------------------------------------------------------------------------------
TimelineWindow::Row::~Row()
{
  SeqDelete(&effects);
}
