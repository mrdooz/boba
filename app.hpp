#pragma once

#include "utils.hpp"

/*
repeat after me: directx is left-handed. z goes into the screen.
*/

namespace FMOD
{
  class System;
  class Sound;
  class Channel;
}

namespace boba
{
  enum class MessageType
  {
    Debug,
    Info,
    Warning,
    Error,
  };

  class App
  {
  public:

    static App& Instance();

    bool Init(HINSTANCE hinstance);
    void Run();

    static bool Create();
    static bool Destroy();

    void AddMessage(MessageType type, const string& str);

  private:
    DISALLOW_COPY_AND_ASSIGN(App);
    App();
    ~App();

    void FindAppRoot();

    void SaveSettings();
    void LoadSettings();

    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    void UpdateMessages();

  protected:

    static App* _instance;
    HINSTANCE _hinstance;

    string _appRoot;
    string _appRootFilename;

#if WITH_MUSIC
    FMOD::System *_system;
    FMOD::Sound  *_sound;
    FMOD::Channel *_channel;
#endif

    struct Message
    {
      wstring str;
      ptime endTime;
      float r, g, b;
    };

    vector<Message> _messages;
  };

#define APP boba::App::Instance()
}
