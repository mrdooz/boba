#pragma once

#include "style_factory.hpp"

namespace effect
{
  namespace protocol
  {
    class EffectSettings;
  }
}

namespace editor
{
  struct Plexus;
  struct StyleFactory;
  struct EffectRow;
  struct EffectRowEvent;
  struct RowVar;

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
    virtual void Update();
    void Reset(const effect::protocol::EffectSettings& settings);
    void AddDefaultRows(bool addKeyframes);

    int TimeToPixel(const time_duration& t) const;
    int TimeToPixel(s64 ms) const;
    time_duration PixelToTime(int x) const;
    s64 PixelToTimeMs(int x) const;
    time_duration AbsPixelToTime(int x) const;
    void UpdateStatusBar(int segment, const string& value);

    void KeyframesModified();

    DisplayMode GetDisplayMode() const { return _displayMode; }
    void SendEffectEvent(RowVar* sender, const EffectRowEvent& event);

    template<typename T>
    sf::Vector2<T> PointToLocal(int x, int y)
    {
      return sf::Vector2<T>((T)(x - _pos.x), (T)(y - _pos.y));
    }

    static TimelineWindow* _instance;

  private:
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