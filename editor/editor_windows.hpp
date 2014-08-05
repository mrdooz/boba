#pragma once

namespace editor
{

  struct Plexus;

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

    void DrawPlexus(const Plexus& plexus);

    struct Effect {

    };

    struct ModuleFlagsF
    {
      enum Enum { Selected = 1 << 0, Dragging = 1 << 1, InvalidDrop = 1 << 2, };
      struct Bits { u32 selected : 1; u32 dragging : 1; u32 invalidDrop : 1; };
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
      enum Enum { Expanded = 1 << 0 };
      struct Bits { u32 expanded : 1; };
    };

    typedef Flags<RowFlagsF> RowFlags;

    struct EffectInstanceFlagsF
    {
      enum Enum { Move = 1 << 0, ResizeStart = 1 << 1, ResizeEnd = 1 << 2,  Selected = 1 << 3 };
      struct Bits { u32 move : 1; u32 resizeStart : 1; u32 resizeEnd : 1; u32 selected : 1; };
    };

    typedef Flags<EffectInstanceFlagsF> EffectInstanceFlags;

    struct Row;

    struct EffectRow
    {
      struct RowFlagsF
      {
        enum Enum { Expanded = 1 << 0, Selected = 1 << 1 };
        struct Bits { u32 expanded : 1; u32 selected; };
      };

      typedef Flags<RowFlagsF> RowFlags;

      EffectRow(const Font& font);
      EffectRow(const Font& font, const string& str, const IntRect& bounds);

      void Draw(RenderTexture& texture);

      string str;
      IntRect bounds;
      RowFlags flags;
      EffectRow* parent;
      Text text;
    };

    struct EffectInstance
    {
      time_duration startTime;
      time_duration endTime;
      Module* module;
      Row* row;
      EffectInstanceFlags flags;
    };

    struct Row
    {
      ~Row();
      // adjust start/ends values so they don't overlap with any
      // existing effects
      void TimeFixup(time_duration* start, time_duration* end, EffectInstance* cur) const;
      bool IsValidPosition(const time_duration& t, EffectInstance* cur) const;
      time_duration SnappedPosition(const time_duration& t, bool start, EffectInstance* cur) const;
      void AddEffect(EffectInstance* effect);
      u32 id;
      IntRect rect;
      RowFlags flags;
      vector<EffectInstance*> effects;
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

    void DrawModule(float x, float y, const Module& m);
    void DrawEffect(const EffectInstance& effect, const Row& row, Text& text);


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
    vector<Module*> _modules;
    vector<Row*> _rows;

    TimelineFlags _timelineFlags;

    Module* _selectedModule;
    EffectInstance* _selectedEffect;
    Vector2i _lastDragPos;

    vector<EffectRow*> _effectRows;
  };


}