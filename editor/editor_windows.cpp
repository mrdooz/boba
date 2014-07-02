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
    : VirtualWindow(title, pos, size, make_shared<sf::RenderTexture>(), flags)
{
}

//----------------------------------------------------------------------------------
PreviewWindow::PreviewWindow(
  const string& title,
  const Vector2f& pos,
  const Vector2f& size,
  const bristol::WindowFlags& flags)
    : VirtualWindow(title, pos, size, make_shared<sf::RenderTexture>(), flags)
{
}


//----------------------------------------------------------------------------------
ComponentWindow::ComponentWindow(
  const string& title,
  const Vector2f& pos,
  const Vector2f& size,
  const bristol::WindowFlags& flags)
    : VirtualWindow(title, pos, size, make_shared<sf::RenderTexture>(), flags)
{
}

//----------------------------------------------------------------------------------
TimelineWindow::TimelineWindow(
  const string& title,
  const Vector2f& pos,
  const Vector2f& size,
  const bristol::WindowFlags& flags)
    : VirtualWindow(title, pos, size, make_shared<sf::RenderTexture>(), flags)
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

  _font.loadFromFile("gfx/04b_03b_.ttf");
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
  //_texture->clear();

  const editor::Settings& settings = EDITOR.Settings();

  // draw the ticker
  RectangleShape ticker;
  ticker.setFillColor(Color(40, 40, 40));
  ticker.setSize(Vector2f(_size.x, settings.ticker_height()));
  _texture->draw(ticker);

  time_duration t = EDITOR.CurTime();
  Text curTime(to_string("XXXX %d", t.total_milliseconds()), _font);
  curTime.setPosition(10, 10);
  curTime.setCharacterSize(20);
  _texture->draw(curTime);

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
  _texture->draw(lines);

  VertexArray curLine(sf::Lines);
  int w = _size.x;
  while (TimeToPixel(t - _panelOffset) > w)
  {
    _panelOffset += PixelToTime(w);
  }
  x = TimeToPixel(EDITOR.CurTime() - _panelOffset);
  curLine.append(sf::Vertex(Vector2f(x, y), Color::Red));
  curLine.append(sf::Vertex(Vector2f(x, _size.y), Color::Red));
  _texture->draw(curLine);

  // draw the rows

  //_texture.display();
}

