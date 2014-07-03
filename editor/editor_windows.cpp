#include "editor_windows.hpp"
#include "editor.hpp"
#include "proto_utils.hpp"
#import "flags.hpp"

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
    , _draggingTemplate(nullptr)
    , _selectedTemplate(nullptr)
    , _hoverRow(nullptr)
{
}

//----------------------------------------------------------------------------------
void TimelineWindow::ResetDragDrop()
{
  if (_hoverRow)
    _hoverRow->flags.Clear(RowFlagsF::Hover);

  _draggingTemplate = nullptr;
  _selectedTemplate = nullptr;
  _hoverRow = nullptr;
  _timelineFlags.Clear(TimelineFlagsF::PendingDrag);
}

//----------------------------------------------------------------------------------
bool TimelineWindow::OnMouseButtonPressed(const Event& event)
{
  ResetDragDrop();

  int x = event.mouseButton.x - _pos.x;
  int y = event.mouseButton.y - _pos.y;
  if (x < EDITOR.Settings().module_view_width())
  {
    for (EffecTemplate & m : _effectTemplates)
    {
      m.flags.Clear(EffectTemplateFlagsF::Selected);
    }

    // handle component click
    for (EffecTemplate & m : _effectTemplates)
    {
      if (m.rect.contains(x, y))
      {
        m.flags.Set(EffectTemplateFlagsF::Selected);
        _selectedTemplate = &m;
        _timelineFlags.Set(TimelineFlagsF::PendingDrag);
        break;
      }
    }
  }
  else
  {
    time_duration t = PixelToTime(event.mouseButton.x - _pos.x) + _panelOffset;
    if (!t.is_not_a_date_time())
    {
      EDITOR.SetCurTime(t);
    }
  }
  return true;
}

//----------------------------------------------------------------------------------
bool TimelineWindow::OnMouseMoved(const Event& event)
{
  if (_timelineFlags.IsSet(TimelineFlagsF::PendingDrag))
  {
    _draggingTemplate = _selectedTemplate;
    _timelineFlags.Clear(TimelineFlagsF::PendingDrag);
  }

  if (_draggingTemplate)
  {
    _dragPos = PointToLocal<int>(event.mouseMove.x, event.mouseMove.y);

    if (_hoverRow)
      _hoverRow->flags.Clear(RowFlagsF::Hover);

    for (Row& row : _rows)
    {
      if (row.rect.contains(_dragPos.x, _dragPos.y))
      {
        _hoverRow = &row;
        _hoverRow->flags.Set(RowFlagsF::Hover);
        break;
      }
    }
  }
  return true;
}

//----------------------------------------------------------------------------------
bool TimelineWindow::OnMouseButtonReleased(const Event& event)
{
  if (_draggingTemplate)
  {
    // check if module dropped on any row
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
    _effectTemplates.push_back({i, effect.name, IntRect(Vector2i(0, y), moduleSize)});
    y += settings.module_row_height() - 2;
  }


  return true;
}
//----------------------------------------------------------------------------------
void TimelineWindow::DrawTimeline()
{
  const editor::Settings& settings = EDITOR.Settings();

  Color rowCol = FromProtocol(settings.default_row_color());
  Color hoverCol = FromProtocol(settings.hover_row_color());

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
  u32 rowHeight = settings.module_row_height();
  x = settings.module_view_width();
  y = settings.ticker_height();
  for (const Row& row : _rows)
  {
    DrawRectOutline(_texture, Vector2f(row.rect.left, row.rect.top), Vector2f(row.rect.width, row.rect.height),
        row.flags.IsSet(RowFlagsF::Hover) ? hoverCol : rowCol, 2);
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


}

void TimelineWindow::DrawModule(float x, float y, const EffecTemplate & module)
{
  const editor::Settings& settings = EDITOR.Settings();
  Color rowCol = FromProtocol(settings.default_row_color());
  Color selectedRowCol = FromProtocol(settings.selected_row_color());

  Vector2f size(module.rect.width, module.rect.height);
  Color col = module.flags.IsSet(EffectTemplateFlagsF::Selected) ? selectedRowCol : rowCol;
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
void TimelineWindow::DrawComponents()
{
  const editor::Settings& settings = EDITOR.Settings();
  Color rowCol = FromProtocol(settings.default_row_color());
  Color selectedRowCol = FromProtocol(settings.selected_row_color());

  u32 rowHeight = settings.module_row_height();
  int x = 0;
  int y = settings.module_row_height() - 2;
  int w = settings.module_view_width();

  Text text;
  for (const EffecTemplate & module : _effectTemplates)
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
    return time_duration();

  return milliseconds(1000 * x / _pixelsPerSecond);
}

//----------------------------------------------------------------------------------
void TimelineWindow::Draw()
{
  _texture.clear();

  DrawComponents();
  DrawTimeline();

  if (_draggingTemplate)
    DrawModule(_dragPos.x, _dragPos.y, *_draggingTemplate);

  _texture.display();
}

