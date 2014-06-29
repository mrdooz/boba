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

    virtual void Draw();

  private:
    ptime _startTime;
    u32 _pixelsPerSecond;
  };


}