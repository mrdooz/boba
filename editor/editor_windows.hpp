#include "style_factory.hpp"

#pragma once

namespace editor
{

  struct Plexus;
  struct StyleFactory;
  struct EffectRow;
  struct EffectRowEvent;
  struct RowVar;

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
    friend struct EffectRow;
  public:

    enum class DisplayMode
    {
      Keyframe,
      Graph,
      COUNT,
    };

    TimelineWindow(
        const string& title,
        const Vector2f& pos,
        const Vector2f& size);
    ~TimelineWindow();

    virtual bool Init();
    virtual void Draw();
    void Reset(const effect::protocol::EffectSettings& settings);
    void AddDefaultRows();

    int TimeToPixel(const time_duration& t) const;
    int TimeToPixel(s64 ms) const;
    time_duration PixelToTime(int x) const;
    s64 PixelToTimeMs(int x) const;
    time_duration AbsPixelToTime(int x) const;
    void UpdateStatusBar(int segment, const string& value);

    void KeyframesModified();

    void SendEffectEvent(RowVar* sender, const EffectRowEvent& event);

    DisplayMode GetDisplayMode() const { return _displayMode; }

    template<typename T>
    sf::Vector2<T> PointToLocal(int x, int y)
    {
      return sf::Vector2<T>((T)(x - _pos.x), (T)(y - _pos.y));
    }

    static TimelineWindow* _instance;

  private:

    template <typename T>
    T NextEnum(T t)
    {
      return (T)(((u32)t + 1) % (u32)T::COUNT);
    }

    void DrawEffects();
    void DrawTimeline();
    void DrawTimelinePost();
    void DrawStatusBar();
    void DrawBackground();

    bool OnMouseButtonPressed(const Event& event);
    bool OnMouseMoved(const Event& event);
    bool OnMouseButtonReleased(const Event& event);
    bool OnMouseWheelMoved(const Event& event);
    bool OnKeyReleased(const Event& event);

    void RecalcEffecRows();

    time_duration _panelOffset;
    Font _font;

    vector<EffectRow*> _effectRows;

    Vector2i _lastDragPos;
    StyledRect _tickerRect;
    StyledRect _statusBar;
    vector<string> _statusBarValues;
    DisplayMode _displayMode;
    RowVar* _selectedVar;

    vector<time_duration> _tickIntervals;
    int _curZoom;
  };

}