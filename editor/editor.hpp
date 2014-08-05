#pragma once

#pragma warning(push)
#pragma warning(disable: 4244 4267)
#include "protocol/editor_settings.pb.h"
#include "protocol/effect_settings.pb.h"
#include "protocol/effect_settings_plexus.pb.h"
#pragma warning(pop)

#include "effect_utils.hpp"

namespace bristol
{
  class WindowEventManager;
  class VirtualWindowManager;
}

namespace editor
{
  class Editor
  {
  public:

    struct StateFlagsF
    {
      enum Enum { Done = 1 << 0, Paused = 1 << 1, };
      struct Bits { u32 done : 1; u32 paused : 1; };
    };

    typedef Flags<StateFlagsF> StateFlags;

    static void Create();
    static void Destroy();
    static Editor& Instance();

    bool Init();
    bool Run();
    bool Close();

    editor::protocol::Settings& Settings() { return _settings; }
    string GetAppRoot() const { return _appRoot; }
    time_duration CurTime() const { return _curTime; }
    void SetCurTime(const time_duration& t);

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
    editor::protocol::Settings _settings;
    Plexus _plexus;
    string _appRoot;
    ptime _lastUpdate;
    time_duration _curTime;
    time_duration _fileWatchAcc;
    StateFlags _stateFlags;

    FileWatcher _fileWatcher;
  };

  #define EDITOR Editor::Instance()
}