#pragma once

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

  struct WebsocketClient;
  class App
  {
  public:

    static App& Instance();

    bool Init(HINSTANCE hinstance);
    bool Run();

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

    void ProcessPayload(const void* payload, u32 size);

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
      TimeStamp endTime;
      float r, g, b;
    };

    vector<Message> _messages;

    WebsocketClient* _client;
  };

#define APP boba::App::Instance()
}
