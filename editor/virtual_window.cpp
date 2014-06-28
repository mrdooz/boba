#include "virtual_window.hpp"
#include "utils.hpp"
#include "sfml_helpers.hpp"

using namespace editor;

//-----------------------------------------------------------------------------
VirtualWindow::VirtualWindow(
    const string& title,
    const Vector2f& pos,
    const Vector2f& size,
    const WindowFlags& flags)
  : _windowManager(nullptr)
  , _title(title)
  , _pos(pos)
  , _size(size)
  , _depth(0)
  , _borderWidth(4)
  , _titleBarHeight(10)
  , _focus(false)
  , _moving(false)
  , _resizing(false)
  , _windowFlags(flags)
{
  _texture.create((int)size.x, (int)size.y);
  _sprite.setTexture(_texture.getTexture());
  _sprite.setPosition(pos);

  _defaultColor = Color(rand() % 255, rand() % 255, rand() % 255);
}

//-----------------------------------------------------------------------------
bool VirtualWindow::Init()
{
  if (!_font.loadFromFile("gfx/wscsnrg.ttf"))
    return false;

  return true;
}

//-----------------------------------------------------------------------------
void VirtualWindow::Draw()
{
  _texture.clear();

  RectangleShape rect;
  rect.setPosition(_pos);
  rect.setSize(_size);
  rect.setFillColor(_defaultColor);
  _texture.draw(rect);

  _texture.display();
}

//-----------------------------------------------------------------------------
void VirtualWindow::SetSize(const Vector2f& size)
{
  _size = size;
  _texture.create((int)size.x, (int)size.y);
  _sprite.setTexture(_texture.getTexture(), true);
}

//-----------------------------------------------------------------------------
void VirtualWindow::SetPosition(const Vector2f& pos)
{
  _pos = pos;
  _sprite.setPosition(pos);
}

//-----------------------------------------------------------------------------
void VirtualWindow::DrawBorder(RenderWindow* window)
{
  bool drawBorder = _windowFlags.IsSet(WindowFlag::DrawBorder);

  float w = drawBorder ? (float)_borderWidth : 0;

  if (drawBorder)
  {
    RoundedRectangleShape rect(_size + Vector2f(2*w, w+_titleBarHeight), 10, 10, 0, 0, 40);
    rect.setPosition(_pos - Vector2f(w, 10));
    rect.setFillColor(_focus ? Color(50, 50, 50) : Color(30, 30, 30));
    window->draw(rect);
  }

  // center the text (taking border into account)
  if (_windowFlags.IsSet(WindowFlag::DrawTitle))
  {
    Text text(_title, _font, _titleBarHeight - 2);
    float tw = text.getLocalBounds().width;
    text.setPosition(_pos + Vector2f(w + (_size.x - tw) / 2, _titleBarHeight));
    text.setColor(Color::White);
    window->draw(text);
  }
}

//-----------------------------------------------------------------------------
bool VirtualWindow::PointInside(const Vector2f& pos, bool includeBorder)
{
  float w = includeBorder ? (float)_borderWidth : 0;
  float h = includeBorder ? (float)_titleBarHeight : 0;

  FloatRect rect(_pos - Vector2f(w, h),
    Vector2f(2*w + _size.x, h + _size.y + w));

  return rect.contains(pos);
}

//-----------------------------------------------------------------------------
u32 VirtualWindow::PointInsideBorder(const Vector2f& pos)
{
  //enum { LeftBorder = 1, RightBorder = 2, TopBorder = 4, BottomBorder = 8};

  float w = (float)_borderWidth;
  float h = (float)_titleBarHeight;

  u32 mask = 0;

  FloatRect rectLeft(_pos - Vector2f(w, h), Vector2f(w, h+_size.y+w));
  if (rectLeft.contains(pos))
    mask |= LeftBorder;

  FloatRect rectRight(_pos + Vector2f(_size.x, -h), Vector2f(w, h+_size.y+w));
  if (rectRight.contains(pos))
    mask |= RightBorder;

  FloatRect rectTop(_pos - Vector2f(w, h), Vector2f(2*w + _size.x, h));
  if (rectTop.contains(pos))
    mask |= TopBorder;

  FloatRect rectTopOuter(_pos - Vector2f(w, h), Vector2f(2*w + _size.x, w));
  if (rectTopOuter.contains(pos))
    mask |= TopBorderOuter;

  FloatRect rectBottom(_pos + Vector2f(-w, _size.y), Vector2f(2*w + _size.x, w));
  if (rectBottom.contains(pos))
    mask |= BottomBorder;

  return mask;
}

//-----------------------------------------------------------------------------
GridSplitter::GridSplitter(
  VirtualWindow* topLeft,
  VirtualWindow* topRight,
  VirtualWindow* bottomLeft,
  VirtualWindow* bottomRight,
  const Vector2f& size,
  const Vector2f& ratio)
    : VirtualWindow("", Vector2f(0,0), size, WindowFlags(WindowFlag::Resizable))
    , _ratio(ratio)
{
  _windows[0] = topLeft;
  _windows[1] = topRight;
  _windows[2] = bottomLeft;
  _windows[3] = bottomRight;


  // set the initial position and size
  float w = ratio.x * size.x;
  float rw = size.x - w;
  float h = ratio.y * size.y;
  float rh = size.y - h;

  _topLeft->SetSize(Vector2f(w, h));
  _topLeft->SetPosition(Vector2f(0,0));
  _sprites[0].setPosition(Vector2f(0, 0));
  _sprites[0].setTexture(_topLeft->GetTexture()->getTexture());

  _topRight->SetSize(Vector2f(rw, h));
  _topRight->SetPosition(Vector2f(0, 0));
  _sprites[1].setPosition(Vector2f(w, 0));
  _sprites[1].setTexture(_topRight->GetTexture()->getTexture());

  _bottomLeft->SetSize(Vector2f(w, rh));
  _bottomLeft->SetPosition(Vector2f(0, 0));
  _sprites[2].setPosition(Vector2f(0, h));
  _sprites[2].setTexture(_bottomLeft->GetTexture()->getTexture());

  _bottomRight->SetSize(Vector2f(rw, rh));
  _bottomRight->SetPosition(Vector2f(0, 0));
  _sprites[3].setPosition(Vector2f(w, h));
  _sprites[3].setTexture(_bottomRight->GetTexture()->getTexture());
}

//-----------------------------------------------------------------------------
void GridSplitter::Draw()
{
  _texture.clear();

  // render the windows to their render targets
  for (VirtualWindow* w : _windows)
    w->Draw();

  // render the splitter sprites
  for (const Sprite& s : _sprites)
    _texture.draw(s);

  _texture.display();
}

