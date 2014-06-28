#include "editor_windows.hpp"

using namespace editor;

//----------------------------------------------------------------------------------
PropertyWindow::PropertyWindow(
  const string& title,
  const Vector2f& pos,
  const Vector2f& size,
  const WindowFlags& flags)
    : VirtualWindow(title, pos, size, flags)
{
}

//----------------------------------------------------------------------------------
PreviewWindow::PreviewWindow(
  const string& title,
  const Vector2f& pos,
  const Vector2f& size,
  const WindowFlags& flags)
    : VirtualWindow(title, pos, size, flags)
{
}


//----------------------------------------------------------------------------------
ComponentWindow::ComponentWindow(
  const string& title,
  const Vector2f& pos,
  const Vector2f& size,
  const WindowFlags& flags)
    : VirtualWindow(title, pos, size, flags)
{
}

//----------------------------------------------------------------------------------
TimelineWindow::TimelineWindow(
  const string& title,
  const Vector2f& pos,
  const Vector2f& size,
  const WindowFlags& flags)
    : VirtualWindow(title, pos, size, flags)
{
}
