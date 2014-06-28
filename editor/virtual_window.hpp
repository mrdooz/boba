#pragma once
#include "flags.hpp"

namespace editor
{
  class VirtualWindowManager;

  struct WindowFlag
  {
    enum Enum
    {
      DrawBorder    =   1 << 1,
      DrawTitle     =   1 << 2,
      Movable       =   1 << 3,
      Resizable     =   1 << 4,

      DefaultFlags  =   DrawBorder | DrawTitle | Movable | Resizable
    };

    struct Bits { u32 drawBorder : 1; u32 drawTitle : 1; u32 movable : 1; u32 resizable : 1; };
  };

  typedef Flags<WindowFlag> WindowFlags;

  class VirtualWindow
  {
    friend class VirtualWindowManager;
  public:

    VirtualWindow(
        const string& title,
        const Vector2f& pos,
        const Vector2f& size,
        const WindowFlags& flags = WindowFlags(WindowFlag::DefaultFlags));
    virtual ~VirtualWindow() {}

    // TODO: update with fixed time step. allow windows to specify frequency
    virtual void Update() {}
    virtual bool Init();
    virtual void Draw();
    RenderTexture* GetTexture() { return &_texture; }

    bool PointInside(const Vector2f& pos, bool includeBorder);

    // TopBorderOuter is the outermost part of the top border (the part that is as thick
    // as the other side's borders).
    enum { LeftBorder = 1, RightBorder = 2, TopBorder = 4, TopBorderOuter = 8, BottomBorder = 16};
    u32 PointInsideBorder(const Vector2f& pos);

    void SetPosition(const Vector2f& pos);
    void SetSize(const Vector2f& size);

  protected:

    void DrawBorder(RenderWindow* window);

    VirtualWindowManager* _windowManager;

    Sprite _sprite;
    RenderTexture _texture;

    Font _font;
    Color _defaultColor;

    string _title;
    Vector2f _pos;
    Vector2f _size;
    int _depth;
    u32 _borderWidth;
    u32 _titleBarHeight;
    bool _focus;
    bool _moving;
    bool _resizing;

    WindowFlags _windowFlags;
  };

  class GridSplitter : public VirtualWindow
  {
  public:
    GridSplitter(
      VirtualWindow* topLeft,
      VirtualWindow* topRight,
      VirtualWindow* bottomLeft,
      VirtualWindow* bottomRight,
      const Vector2f& size,
      const Vector2f& ratio = Vector2f(0.5f, 0.5f));

    virtual void Draw();

  private:
    enum Pos { TopLeft, TopRight, BottomLeft, BottomRight };
    union {
      VirtualWindow* _windows[4];
      struct {
        VirtualWindow*_topLeft;
        VirtualWindow*_topRight;
        VirtualWindow*_bottomLeft;
        VirtualWindow*_bottomRight;
      };
    };

    Sprite _sprites[4];
    Vector2f _ratio;
  };
}