#include "editor_windows.hpp"
#include "editor.hpp"

using namespace editor;

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
{
}

//----------------------------------------------------------------------------------
void TimelineWindow::Draw()
{
  _texture.clear();

  const editor::Settings& settings = EDITOR.Settings();
  // draw the ticker
  RectangleShape ticker;
  ticker.setSize(Vector2f(_size.x, settings.ticker_height()));
  _texture.draw(ticker);

  // draw the rows

  _texture.display();
}

