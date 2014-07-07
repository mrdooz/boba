#pragma once

#include "protocol/editor_settings.pb.h"
#include "protocol/descriptor.pb.h"

namespace bristol
{
  class WindowEventManager;
  class VirtualWindowManager;
}

namespace editor
{

  struct Effect
  {
    string name;
    u32 tjong;
  };


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

    editor::Settings& Settings() { return _settings; }
    string GetAppRoot() const { return _appRoot; }
    time_duration CurTime() const { return _curTime; }
    void SetCurTime(const time_duration& t);

    const vector<Effect>& GetEffects() const { return _effects; }

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
    google::protobuf::FileDescriptorSet _descriptorSet;
    string _appRoot;
    ptime _lastUpdate;
    time_duration _curTime;
    time_duration _fileWatchAcc;
    StateFlags _stateFlags;

    vector<Effect> _effects;
    FileWatcher _fileWatcher;
  };

  #define EDITOR Editor::Instance()
}