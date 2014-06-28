#pragma once

#include "protocol/editor_settings.pb.h"

namespace editor
{
  class WindowEventManager;
  class VirtualWindowManager;

  class Editor
  {
  public:

    static void Create();
    static void Destroy();
    static Editor& Instance();

    bool Init();
    bool Run();
    bool Close();

    editor::Settings& Settings() { return _settings; }

  private:
    Editor();
    ~Editor();

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

    static Editor* _instance;
    editor::Settings _settings;
    bool _done;
  };

  #define EDITOR Editor::Instance()
}