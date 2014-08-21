#include "style_factory.hpp"

#pragma once

namespace editor
{

  struct Plexus;
  struct StyleFactory;
  struct EffectRow;

  template <typename T>
  struct ToggleSet
  {
    ToggleSet()
      : it(backingSet.end())
    {

    }

    bool IsEmpty()
    {
      return backingSet.empty();
    }

    void Reset()
    {
      it = backingSet.begin();
    }

    bool Next()
    {
      if (IsEmpty())
        return false;

      // display the next graph, or step to the next row if done
      if ((*it)->NextGraph())
      {
        ++it;
        if (it == backingSet.end())
        {
          Reset();
          return false;
        }
        (*it)->ToggleGraphView(true);
      }

      return true;
    }

    void Toggle(const T& t)
    {
      auto it = find(backingSet.begin(), backingSet.end(), t);
      if (it == backingSet.end())
      {
        backingSet.push_back(t);
      }
      else
      {
        backingSet.erase(it);
      }
      Reset();
    }

    T CurRow()
    {
      return *it;
    }

    vector<T> backingSet;
    typename vector<T>::iterator it;
  };

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

    int TimeToPixel(const time_duration& t) const;
    time_duration PixelToTime(int x) const;
    time_duration AbsPixelToTime(int x) const;
    void UpdateStatusBar(int segment, const string& value);

    void KeyframesModified();

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
    void DrawStatusBar();

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
    bool OnKeyReleased(const Event& event);

    void RecalcEffecRows();

    time_duration _panelOffset;
    u32 _pixelsPerSecond;
    Font _font;

    TimelineFlags _timelineFlags;
    vector<EffectRow*> _effectRows;
    EffectRow* _curRow;

    Vector2i _lastDragPos;
//    EffectRow* _editRow;
    StyledRect _tickerRect;
    StyledRect _statusBar;
    vector<string> _statusBarValues;
//    EffectRow* _movingKeyframe;
//    EffectRow*_selectedRow;
    DisplayMode _displayMode;
//    ToggleSet<EffectRow*> _selectedRows;
  };

}