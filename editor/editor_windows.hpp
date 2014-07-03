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

    struct EffectTemplateFlagsF
    {
      enum Enum { Selected = 1 << 0, Dragging = 1 << 1};
      struct Bits { u32 selected : 1; u32 dragging : 1; };
    };

    typedef Flags<EffectTemplateFlagsF> EffectTemplateFlags;

    struct EffecTemplate
    {
      u32 id;
      string name;
      IntRect rect;
      EffectTemplateFlags flags;
    };

    struct RowFlagsF
    {
      enum Enum { Expanded = 1 << 0, Hover = 1 << 1};
      struct Bits { u32 expanded : 1; u32 hover : 1; };
    };

    typedef Flags<RowFlagsF> RowFlags;

    struct EffectInstance
    {

    };

    struct Row
    {
      u32 id;
      IntRect rect;
      RowFlags flags;
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

    void DrawModule(float x, float y, const EffecTemplate & m);

    void ResetDragDrop();

    template<typename T>
    sf::Vector2<T> PointToLocal(int x, int y)
    {
      return sf::Vector2<T>((T)(x - _pos.x), (T)(y - _pos.y));
    }

    time_duration _panelOffset;
    u32 _pixelsPerSecond;
    Font _font;
    vector<EffecTemplate> _effectTemplates;
    vector<Row> _rows;

    TimelineFlags _timelineFlags;

    EffecTemplate* _draggingTemplate;
    EffecTemplate* _selectedTemplate;
    Row* _hoverRow;
    Vector2i _dragPos;
  };


}