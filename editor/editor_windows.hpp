#include "style_factory.hpp"

#pragma once

namespace editor
{

  struct Plexus;
  struct StyleFactory;

  //----------------------------------------------------------------------------------
  class PropertyWindow : public VirtualWindow
  {
  public:
    PropertyWindow(
        const string& title,
        const Vector2f& pos,
        const Vector2f& size);
  };

  //----------------------------------------------------------------------------------
  class PreviewWindow : public VirtualWindow
  {
  public:
    PreviewWindow(
        const string& title,
        const Vector2f& pos,
        const Vector2f& size);
  };

  //----------------------------------------------------------------------------------
  class TimelineWindow : public VirtualWindow
  {
  public:
    TimelineWindow(
        const string& title,
        const Vector2f& pos,
        const Vector2f& size);
    ~TimelineWindow();

    virtual bool Init();
    virtual void Draw();

    int TimeToPixel(const time_duration& t);
    time_duration PixelToTime(int x);
    time_duration AbsPixelToTime(int x);

  private:

    void DrawEffects();
    void DrawTimeline();

    struct EffectRow
    {
      struct RowFlagsF
      {
        enum Enum { Expanded = 1 << 0, Selected = 1 << 1 };
        struct Bits { u32 expanded : 1; u32 selected; };
      };

      typedef Flags<RowFlagsF> RowFlags;

      EffectRow(
          const Font& font,
          const string& str,
          const IntRect& bounds,
          EffectRow* parent = nullptr);

      void Draw(RenderTexture& texture);

      string str;
      RowFlags flags;
      EffectRow* parent;
      StyledRectangle* rect;
      vector<EffectRow*> children;
      Text text;
      int level;
    };

    struct TimelineFlagsF
    {
      enum Enum { PendingDrag = 1 << 0, PendingEffectMove = 1 << 1};
      struct Bits { u32 pendingDrag : 1; u32 pendingEffectMove : 1; };
    };

    typedef Flags<TimelineFlagsF> TimelineFlags;


    bool OnMouseButtonPressed(const Event& event);
    bool OnMouseMoved(const Event& event);
    bool OnMouseButtonReleased(const Event& event);
    bool OnMouseWheelMoved(const Event& event);

    template<typename T>
    sf::Vector2<T> PointToLocal(int x, int y)
    {
      return sf::Vector2<T>((T)(x - _pos.x), (T)(y - _pos.y));
    }

    Sprite _effectSprite;
    Sprite _timelineSprite;
    RenderTexture _effectTexture;
    RenderTexture _timelineTexture;

    time_duration _panelOffset;
    u32 _pixelsPerSecond;
    Font _font;

    TimelineFlags _timelineFlags;
    vector<EffectRow*> _effectRows;

    Vector2i _lastDragPos;

  };

}