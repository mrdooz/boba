#pragma once

namespace editor
{
  class WindowEventManager;
  class VirtualWindowManager;

  class Editor
  {
  public:
    Editor();
    ~Editor();

    bool Init();
    bool Run();
    bool Close();

  private:

    void Update();
    void Render();

    bool OnLostFocus(const Event& event);
    bool OnGainedFocus(const Event& event);
    bool OnKeyPressed(const Event& event);
    bool OnKeyReleased(const Event& event);

    bool OnMouseButtonReleased(const Event& event);

    RenderWindow* _renderWindow;
    WindowEventManager* _eventManager;
    VirtualWindowManager* _virtualWindowManager;

    bool _done;
  };

}