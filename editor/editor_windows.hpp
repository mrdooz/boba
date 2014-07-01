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
        const Vector2f& size,
        const bristol::WindowFlags& = bristol::WindowFlags(bristol::WindowFlag::DefaultFlags));
  };

  //----------------------------------------------------------------------------------
  class PreviewWindow : public VirtualWindow
  {
  public:
    PreviewWindow(
        const string& title,
        const Vector2f& pos,
        const Vector2f& size,
        const bristol::WindowFlags& = bristol::WindowFlags(bristol::WindowFlag::DefaultFlags));
  };

  //----------------------------------------------------------------------------------
  class ComponentWindow : public VirtualWindow
  {
  public:
    ComponentWindow(
        const string& title,
        const Vector2f& pos,
        const Vector2f& size,
        const bristol::WindowFlags& = bristol::WindowFlags(bristol::WindowFlag::DefaultFlags));
  };

  //----------------------------------------------------------------------------------
  class TimelineWindow : public VirtualWindow
  {
  public:
    TimelineWindow(
        const string& title,
        const Vector2f& pos,
        const Vector2f& size,
        const bristol::WindowFlags& = bristol::WindowFlags(bristol::WindowFlag::DefaultFlags));

    virtual bool Init();
    virtual void Draw();

    int TimeToPixel(const time_duration& t);
    time_duration PixelToTime(int x);

  private:
    time_duration _panelOffset;
    time_duration _curTime;
    u32 _pixelsPerSecond;
    Font _font;
  };


}