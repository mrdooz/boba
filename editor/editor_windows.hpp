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
  class ComponentWindow : public VirtualWindow
  {
  public:
    ComponentWindow(
        const string& title,
        const Vector2f& pos,
        const Vector2f& size);

    virtual bool Init();
    virtual void Draw();

    bool OnMouseButtonPressed(const Event& event);
    bool OnMouseMoved(const Event& event);
    bool OnMouseButtonReleased(const Event& event);

  private:

    struct ModuleFlagsF
    {
      enum Enum { Selected = 1 << 0, };
      struct Bits { u32 selected : 1; };
    };

    typedef Flags<ModuleFlagsF> ModuleFlags;

    struct Module
    {
      int id;
      string name;
      ModuleFlags flags;
    };

    vector<Module> _modules;

    Font _font;
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

    struct RowFlagsF
    {
      enum Enum { Expanded = 1 << 0, };
      struct Bits { u32 expanded : 1; };
    };

    typedef Flags<RowFlagsF> RowFlags;

    struct Row
    {
      int id;
      RowFlags flags;
    };

    bool OnMouseButtonPressed(const Event& event);

    time_duration _panelOffset;
    u32 _pixelsPerSecond;
    Font _font;
    vector<Row> _rows;
  };


}