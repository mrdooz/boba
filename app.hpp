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
  extern Vector4 DEBUG_INFO_COLOR;
  extern Vector4 DEBUG_WARNING_COLOR;
  extern Vector4 DEBUG_ERROR_COLOR;

  class App
  {
  public:

    static App& Instance();

    bool Init(HINSTANCE hinstance);
    void Run();

    static bool Create();
    static bool Destroy();

    const string& AppRoot() const { return _appRoot; }

  private:
    DISALLOW_COPY_AND_ASSIGN(App);
    App();
    ~App();

    void FindAppRoot();

    void SaveSettings();
    void LoadSettings();

    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

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
  };

#define APP boba::App::Instance()
}
