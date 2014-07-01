#include "editor_windows.hpp"
#include "editor.hpp"

using namespace editor;
using namespace bristol;

//----------------------------------------------------------------------------------
PropertyWindow::PropertyWindow(
  const string& title,
  const Vector2f& pos,
  const Vector2f& size,
  const bristol::WindowFlags& flags)
    : VirtualWindow(title, pos, size, flags)
{
}

//----------------------------------------------------------------------------------
PreviewWindow::PreviewWindow(
  const string& title,
  const Vector2f& pos,
  const Vector2f& size,
  const bristol::WindowFlags& flags)
    : VirtualWindow(title, pos, size, flags)
{
}


//----------------------------------------------------------------------------------
ComponentWindow::ComponentWindow(
  const string& title,
  const Vector2f& pos,
  const Vector2f& size,
  const bristol::WindowFlags& flags)
    : VirtualWindow(title, pos, size, flags)
{
}

//----------------------------------------------------------------------------------
TimelineWindow::TimelineWindow(
  const string& title,
  const Vector2f& pos,
  const Vector2f& size,
  const bristol::WindowFlags& flags)
    : VirtualWindow(title, pos, size, flags)
    , _panelOffset(seconds(0))
    , _curTime(seconds(0))
    , _pixelsPerSecond(100)
{
}

//----------------------------------------------------------------------------------
bool TimelineWindow::Init()
{
  if (!VirtualWindow::Init())
    return false;

  _font.loadFromFile(EDITOR.GetAppRoot() + "/gfx/04b_03b_.ttf");
  return true;
}

//----------------------------------------------------------------------------------
int TimelineWindow::TimeToPixel(const time_duration& t)
{
  return t.total_milliseconds() / (1000 * _pixelsPerSecond);
}

//----------------------------------------------------------------------------------
time_duration TimelineWindow::PixelToTime(int x)
{
  // p = t / (1000 * s);
  // (1000 * s) * p
  return milliseconds(x * 1000 * _pixelsPerSecond);
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

  Text curTime;
  curTime.setFont(_font);
  curTime.setScale(10, 10);
  curTime.setPosition(10, 10);
  curTime.setString(to_string("%d", _curTime.total_milliseconds()));
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
  x = TimeToPixel(EDITOR.CurTime());
  curLine.append(sf::Vertex(Vector2f(x, 0), Color::Red));
  curLine.append(sf::Vertex(Vector2f(x, 40), Color::Red));
  _texture.draw(curLine);


  // draw the rows

  _texture.display();
}

