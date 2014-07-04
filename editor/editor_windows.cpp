#include "editor_windows.hpp"
#include "editor.hpp"
#include "proto_utils.hpp"

using namespace editor;
using namespace bristol;

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
    , _panelOffset(seconds(0))
    , _pixelsPerSecond(100)
    , _selectedModule(nullptr)
    , _hoverRow(nullptr)
{
}

//----------------------------------------------------------------------------------
void TimelineWindow::ResetDragDrop()
{
  _draggingModule.Reset();
  _selectedModule = nullptr;
  _hoverRow = nullptr;
  _timelineFlags.Clear(TimelineFlagsF::PendingDrag);

  _draggingEffect.Reset();
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
    for (Module & m : _modules)
    {
      m.flags.Clear(ModuleFlagsF::Selected);
    }

    // handle component click
    for (Module & m : _modules)
    {
      if (m.rect.contains(x, y))
      {
        m.flags.Set(ModuleFlagsF::Selected);
        _selectedModule = &m;
        _timelineFlags.Set(TimelineFlagsF::PendingDrag);
        break;
      }
    }
  }
  else if (y < (int)settings.module_row_height())
  {
    time_duration t = PixelToTime(event.mouseButton.x - (int)_pos.x) + _panelOffset;
    if (!t.is_not_a_date_time())
    {
      EDITOR.SetCurTime(t);
    }
  }
  else
  {
    // effect movement
    for (Row& row : _rows)
    {
      for (EffectInstance* e : row.effects)
      {
        e->flags.Clear(EffectInstanceFlagsF::Selected);
      }

      if (!row.rect.contains(x, y))
        continue;

      time_duration t = PixelToTime(x);
      for (EffectInstance* e : row.effects)
      {
        if (t >= e->startTime && t < e->endTime)
        {
          e->flags.Set(EffectInstanceFlagsF::Selected);
          _draggingEffect.startPos = x;
          _draggingEffect.orgStart = e->startTime;
          _draggingEffect.orgEnd = e->endTime;
          _draggingEffect.effect = e;
        }
      }
    }

  }
  return true;
}

//----------------------------------------------------------------------------------
bool TimelineWindow::OnMouseMoved(const Event& event)
{
  // should we start module drag/drop
  if (_timelineFlags.IsSet(TimelineFlagsF::PendingDrag))
  {
    _draggingModule.module = _selectedModule;
    _timelineFlags.Clear(TimelineFlagsF::PendingDrag);
  }

  Vector2i pos = PointToLocal<int>(event.mouseMove.x, event.mouseMove.y);
  time_duration curTime = PixelToTime(pos.x);

  // check if any module is being dragged
  if (_draggingModule.module)
  {

    // check if the module is above any row
    for (Row& row : _rows)
    {
      if (row.rect.contains(pos))
      {
        _hoverRow = &row;
        // check if there is space for the current module
        _draggingModule.dragStart = curTime;
        _draggingModule.dragEnd = curTime + seconds(5);
        row.TimeFixup(&_draggingModule.dragStart, &_draggingModule.dragEnd, nullptr);
        _draggingModule.dragPos.x = TimeToPixel(_draggingModule.dragStart);
        _draggingModule.dragPos.y = row.rect.top;

        break;
      }
    }
  }
  else if (_draggingEffect.effect)
  {
    // calc delta, and apply
    time_duration delta = AbsPixelToTime(pos.x - _draggingEffect.startPos);
    EffectInstance* e = _draggingEffect.effect;
    e->startTime = _draggingEffect.orgStart + delta;
    e->endTime = _draggingEffect.orgEnd + delta;
    e->row->TimeFixup(&e->startTime, &e->endTime, e);
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
    for (Row& row : _rows)
    {
      if (row.rect.contains(pos))
      {
        row.AddEffect(new EffectInstance { _draggingModule.dragStart, _draggingModule.dragEnd, _draggingModule.module, &row });
        break;
      }
    }
  }

  ResetDragDrop();
  return true;
}

//----------------------------------------------------------------------------------
bool TimelineWindow::Init()
{
  _windowManager->RegisterHandler(Event::MouseButtonPressed, nullptr, bind(&TimelineWindow::OnMouseButtonPressed, this, _1));
  _windowManager->RegisterHandler(Event::MouseMoved, nullptr, bind(&TimelineWindow::OnMouseMoved, this, _1));
  _windowManager->RegisterHandler(Event::MouseButtonReleased, nullptr, bind(&TimelineWindow::OnMouseButtonReleased, this, _1));

  if (!VirtualWindow::Init())
    return false;

  if (!_font.loadFromFile(EDITOR.GetAppRoot() + "gfx/04b_03b_.ttf"))
    return false;

  const vector<Effect>& effects = EDITOR.GetEffects();
  const editor::Settings& settings = EDITOR.Settings();

  int x = settings.module_view_width();
  int y = settings.module_row_height() - 2;
  Vector2i rowSize(_size.x - settings.module_view_width(), settings.module_row_height());
  for (u32 i = 0; i < 10; ++i)
  {
    _rows.push_back({i, IntRect(Vector2i(x, y), rowSize)});
    y += settings.module_row_height() - 2;
  }


  Vector2i moduleSize(settings.module_view_width(), settings.module_row_height());
  y = settings.module_row_height() - 2;
  for (u32 i = 0; i < effects.size(); ++i)
  {
    const Effect& effect = effects[i];
    _modules.push_back({i, effect.name, IntRect(Vector2i(0, y), moduleSize)});
    y += settings.module_row_height() - 2;
  }


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
  DrawRectOutline(_texture, pos, size, effect.flags.IsSet(EffectInstanceFlagsF::Selected) ? selectedCol : hoverCol, 2, 6);

  text.setString(effect.module->name);
  Vector2f ofs(text.getLocalBounds().width, text.getLocalBounds().height);
  text.setPosition(pos + (size - ofs) / 2.0f);
  _texture.draw(text);

}

//----------------------------------------------------------------------------------
void TimelineWindow::DrawTimeline()
{
  const editor::Settings& settings = EDITOR.Settings();

  Color rowCol = FromProtocol(settings.default_row_color());

  // draw the ticker
  RectangleShape ticker;
  ticker.setFillColor(rowCol);
  ticker.setSize(Vector2f(_size.x, settings.ticker_height()));
  _texture.draw(ticker);

  time_duration t = EDITOR.CurTime();
  Text curTime(to_string("TIME: %.3f", t.total_milliseconds() / 1000.0f), _font);
  curTime.setPosition(10, 0);
  curTime.setCharacterSize(16);
  _texture.draw(curTime);

  VertexArray lines(sf::Lines);
  int x = settings.module_view_width();
  int y = settings.ticker_height() - 25;
  int minorInc = settings.ticker_interval() / settings.ticks_per_interval();
  while (x < _size.x)
  {
    lines.append(sf::Vertex(Vector2f(x, y + 0)));
    lines.append(sf::Vertex(Vector2f(x, y + 20)));
    int tmpX = x;
    for (int i = 0; i < settings.ticks_per_interval(); ++i)
    {
      lines.append(sf::Vertex(Vector2f(x + i * minorInc, y + 5)));
      lines.append(sf::Vertex(Vector2f(x + i * minorInc, y + 15)));
    }
    x = tmpX + settings.ticker_interval();
  }
  _texture.draw(lines);

  // draw the rows
  x = settings.module_view_width();
  y = settings.ticker_height();
  Text text;
  text.setCharacterSize(16);
  text.setFont(_font);
  for (const Row& row : _rows)
  {
    DrawRectOutline(_texture, Vector2f(row.rect.left, row.rect.top), Vector2f(row.rect.width, row.rect.height), rowCol, 2);

    for (const EffectInstance* e : row.effects)
    {
      DrawEffect(*e, row, text);
    }
  }

  // draw time line
  VertexArray curLine(sf::Lines);
  int w = _size.x;
  y = settings.ticker_height() - 25;
  while (TimeToPixel(t - _panelOffset) > w)
  {
    _panelOffset += PixelToTime(w);
  }

  x = TimeToPixel(EDITOR.CurTime() - _panelOffset);
  curLine.append(sf::Vertex(Vector2f(x, y), Color::Red));
  curLine.append(sf::Vertex(Vector2f(x, _size.y), Color::Red));
  _texture.draw(curLine);

  // draw the drag/drop module
  DrawDraggingModule();
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

  DrawRectOutline(_texture, pos, size, col, 2);
  Text text;
  text.setString(_draggingModule.module->name);
  text.setCharacterSize(16);
  text.setFont(_font);
  Vector2f ofs(text.getLocalBounds().width, text.getLocalBounds().height);
  text.setPosition(pos + (size - ofs) / 2.0f);
  _texture.draw(text);
}

//----------------------------------------------------------------------------------
void TimelineWindow::DrawModule(float x, float y, const Module& module)
{
  const editor::Settings& settings = EDITOR.Settings();
  Color rowCol = FromProtocol(settings.default_row_color());
  Color selectedRowCol = FromProtocol(settings.selected_row_color());

  Vector2f size(module.rect.width, module.rect.height);
  Color col = module.flags.IsSet(ModuleFlagsF::Selected) ? selectedRowCol : rowCol;
  DrawRectOutline(_texture, Vector2f(x,y), size, col, 2);
  Text text;
  text.setString(module.name);
  text.setCharacterSize(16);
  text.setFont(_font);
  Vector2f ofs(text.getLocalBounds().width, text.getLocalBounds().height);
  text.setPosition(Vector2f(x, y) + (size - ofs) / 2.0f);
  _texture.draw(text);
}

//----------------------------------------------------------------------------------
void TimelineWindow::DrawModules()
{
  for (const Module& module : _modules)
  {
    DrawModule(module.rect.left, module.rect.top, module);
  }
}

//----------------------------------------------------------------------------------
int TimelineWindow::TimeToPixel(const time_duration& t)
{
  // p = s * t / 1000
  // t = 1000 * p / s
  return (u64)_pixelsPerSecond * t.total_milliseconds() / 1000 + EDITOR.Settings().module_view_width();
}

//----------------------------------------------------------------------------------
time_duration TimelineWindow::PixelToTime(int x)
{
  x -= EDITOR.Settings().module_view_width();
  if (x < 0)
    return time_duration(boost::posix_time::not_a_date_time);

  return milliseconds(1000 * x / _pixelsPerSecond);
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

  DrawModules();
  DrawTimeline();

  _texture.display();
}

//----------------------------------------------------------------------------------
void TimelineWindow::Row::TimeFixup(
    time_duration* start,
    time_duration* end,
    EffectInstance* cur)
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
