#include "editor_windows.hpp"
#include "editor.hpp"

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
ComponentWindow::ComponentWindow(
  const string& title,
  const Vector2f& pos,
  const Vector2f& size)
    : VirtualWindow(title, pos, size, bristol::WindowFlags(bristol::WindowFlag::StaticWindow))
{
}

//----------------------------------------------------------------------------------
bool ComponentWindow::Init()
{
  if (!_font.loadFromFile(EDITOR.GetAppRoot() + "gfx/04b_03b_.ttf"))
    return false;

  _windowManager->RegisterHandler(Event::MouseButtonPressed, nullptr, bind(&ComponentWindow::OnMouseButtonPressed, this, _1));
  _windowManager->RegisterHandler(Event::MouseMoved, nullptr, bind(&ComponentWindow::OnMouseMoved, this, _1));
  _windowManager->RegisterHandler(Event::MouseButtonReleased, nullptr, bind(&ComponentWindow::OnMouseButtonReleased, this, _1));

  u32 id = 0;
  for (const Effect& effect : EDITOR.GetEffects())
  {
    _modules.push_back({id++, effect.name});
  }

  return true;
}

//----------------------------------------------------------------------------------
void ComponentWindow::Draw()
{
  const editor::Settings& settings = EDITOR.Settings();

  _texture.clear();

  u32 rowHeight = settings.module_row_height();
  int x = 0;
  int y = 0;
  Text text;
  for (const Module& module : _modules)
  {
    Color col = module.flags.IsSet(ModuleFlagsF::Selected) ? Color(80, 80, 80) : Color(40, 40, 40);
    DrawRectOutline(_texture, Vector2f(x,y), Vector2f(_size.x, rowHeight), col, 2);
    text.setString(module.name);
    text.setCharacterSize(16);
    text.setFont(_font);
    Vector2f ofs(text.getLocalBounds().width, text.getLocalBounds().height);
    text.setPosition(Vector2f(x, y) + (Vector2f(_size.x, rowHeight) - ofs) / 2.0f);
    _texture.draw(text);
    y += rowHeight - 2;
  }

  _texture.display();
}

//----------------------------------------------------------------------------------
bool ComponentWindow::OnMouseButtonPressed(const Event& event)
{
  return true;
}

//----------------------------------------------------------------------------------
bool ComponentWindow::OnMouseMoved(const Event& event)
{
  return true;
}

//----------------------------------------------------------------------------------
bool ComponentWindow::OnMouseButtonReleased(const Event& event)
{
  return true;
}

//----------------------------------------------------------------------------------
TimelineWindow::TimelineWindow(
  const string& title,
  const Vector2f& pos,
  const Vector2f& size)
    : VirtualWindow(title, pos, size, bristol::WindowFlags(bristol::WindowFlag::StaticWindow))
    , _panelOffset(seconds(0))
    , _pixelsPerSecond(100)
{
}

//----------------------------------------------------------------------------------
bool TimelineWindow::OnMouseButtonPressed(const Event& event)
{
  time_duration t = PixelToTime(event.mouseButton.x - _pos.x) + _panelOffset;
  EDITOR.SetCurTime(t);
  return true;
}

//----------------------------------------------------------------------------------
bool TimelineWindow::Init()
{
  _windowManager->RegisterHandler(Event::MouseButtonPressed, nullptr, bind(&TimelineWindow::OnMouseButtonPressed, this, _1));

  if (!VirtualWindow::Init())
    return false;

  if (!_font.loadFromFile(EDITOR.GetAppRoot() + "gfx/04b_03b_.ttf"))
    return false;

  for (u32 i = 0; i < 10; ++i)
  {
    _rows.push_back({i});
  }

  return true;
}

//----------------------------------------------------------------------------------
int TimelineWindow::TimeToPixel(const time_duration& t)
{
  // p = s * t / 1000
  // t = 1000 * p / s
  return (u64)_pixelsPerSecond * t.total_milliseconds() / 1000;
}

//----------------------------------------------------------------------------------
time_duration TimelineWindow::PixelToTime(int x)
{
  return milliseconds(1000 * x / _pixelsPerSecond);
}

//----------------------------------------------------------------------------------
void TimelineWindow::Draw()
{
  _texture.clear();

  const editor::Settings& settings = EDITOR.Settings();

  // draw the ticker
  RectangleShape ticker;
  ticker.setFillColor(Color(40, 40, 40));
  ticker.setSize(Vector2f(_size.x, settings.ticker_height()));
  _texture.draw(ticker);

  time_duration t = EDITOR.CurTime();
  Text curTime(to_string("TIME: %.3f", t.total_milliseconds() / 1000.0f), _font);
  curTime.setPosition(10, 0);
  curTime.setCharacterSize(16);
  _texture.draw(curTime);

  VertexArray lines(sf::Lines);
  int x = 0;
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

  VertexArray curLine(sf::Lines);
  int w = _size.x;
  while (TimeToPixel(t - _panelOffset) > w)
  {
    _panelOffset += PixelToTime(w);
  }
  x = TimeToPixel(EDITOR.CurTime() - _panelOffset);
  curLine.append(sf::Vertex(Vector2f(x, y), Color::Red));
  curLine.append(sf::Vertex(Vector2f(x, _size.y), Color::Red));
  _texture.draw(curLine);

  // draw the rows
  u32 rowHeight = settings.module_row_height();
  x = 0;
  y = settings.ticker_height();
  for (const Row& row : _rows)
  {
    DrawRectOutline(_texture, Vector2f(x, y), Vector2f(_size.x, rowHeight), Color(40, 40, 40), 2);
    y += rowHeight - 2;

  }

  _texture.display();
}

