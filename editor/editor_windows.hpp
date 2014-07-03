#pragma once

namespace editor
{

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

    virtual bool Init();
    virtual void Draw();

    int TimeToPixel(const time_duration& t);
    time_duration PixelToTime(int x);

  private:

    void DrawComponents();
    void DrawTimeline();

    struct ModuleFlagsF
    {
      enum Enum { Selected = 1 << 0, Dragging = 1 << 1};
      struct Bits { u32 selected : 1; u32 dragging : 1; };
    };

    typedef Flags<ModuleFlagsF> ModuleFlags;

    struct Module
    {
      u32 id;
      string name;
      IntRect rect;
      ModuleFlags flags;
    };

    struct RowFlagsF
    {
      enum Enum { Expanded = 1 << 0, Hover = 1 << 1, InvalidHover = 1 << 2, };
      struct Bits { u32 expanded : 1; u32 hover : 1; u32 invalidHover : 1; };
    };

    typedef Flags<RowFlagsF> RowFlags;

    struct EffectInstance
    {
      time_duration _startTime;
      time_duration _endTime;
      Module * _module;
    };

    struct Row
    {
      // given the start/end, determines an end value
      // that doesn't overlap with any existing effect
      time_duration AvailableSlot(const time_duration& start, const time_duration& end);
      void AddEffect(const EffectInstance& effect);
      u32 id;
      IntRect rect;
      RowFlags flags;
      vector<EffectInstance> _effects;
    };

    struct TimelineFlagsF
    {
      enum Enum { PendingDrag = 1 << 0, };
      struct Bits { u32 pendingDraw : 1; };
    };

    typedef Flags<TimelineFlagsF> TimelineFlags;


    bool OnMouseButtonPressed(const Event& event);
    bool OnMouseMoved(const Event& event);
    bool OnMouseButtonReleased(const Event& event);

    void DrawModule(float x, float y, const Module & m);

    void ResetDragDrop();

    template<typename T>
    sf::Vector2<T> PointToLocal(int x, int y)
    {
      return sf::Vector2<T>((T)(x - _pos.x), (T)(y - _pos.y));
    }

    time_duration _panelOffset;
    u32 _pixelsPerSecond;
    Font _font;
    vector<Module> _modules;
    vector<Row> _rows;

    TimelineFlags _timelineFlags;

    Module* _draggingModule;
    Module* _selectedModule;
    Row* _hoverRow;
    Vector2i _dragPos;
  };


}