#include "app.hpp"
#include "graphics.hpp"
#include "string_utils.hpp"
#include "file_utils.hpp"
#include "demo_engine.hpp"
#include "particle_test.hpp"
#include "scene_test.hpp"
#include "generator_test.hpp"
#include "resource_manager.hpp"
#include "graphics_utils.hpp"
#include "graphics.hpp"
#include "error.hpp"

static const int WM_LOG_NEW_MSG = WM_APP + 1;
static const int WM_APP_CLOSE = WM_APP + 2;

#define FMOD_CHECKED(x) { \
  FMOD_RESULT result = x; \
  if (result != FMOD_OK) { \
  LOG_ERROR_LN("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result)); \
  return false; \
  } \
}

using namespace boba;

App* App::_instance;

const TCHAR* g_AppWindowClass = _T("BobaClass");
const TCHAR* g_AppWindowTitle = _T("boba - neurotica e.f.s");

#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lParam)	((int)(short)LOWORD(lParam))
#endif
#ifndef GET_Y_LPARAM
#define GET_Y_LPARAM(lParam)	((int)(short)HIWORD(lParam))
#endif

#define WITH_MUSIC 0

LogSinkApp* g_logSinkApp;
LogSinkConsole* g_logSinkConsole;

namespace
{
  tm GetLocalTime()
  {
    time_t rawTime;
    tm timeInfo;
    time(&rawTime);
    localtime_s(&timeInfo, &rawTime);
    return timeInfo;
  }
}

//------------------------------------------------------------------------------
App::App()
  : _hinstance(NULL)
#if WITH_MUSIC
  , _sound(nullptr)
  , _system(nullptr)
  , _channel(nullptr)
#endif
{
  g_logSinkConsole = new LogSinkConsole();
  g_logSinkApp = new LogSinkApp();
  FindAppRoot();
}

//------------------------------------------------------------------------------
App::~App()
{
  SAFE_DELETE(g_logSinkConsole);
  SAFE_DELETE(g_logSinkApp);
}

//------------------------------------------------------------------------------
bool App::Create()
{
  if (!_instance)
    _instance = new App();
  return true;
}

//------------------------------------------------------------------------------
bool App::Destroy()
{
#if WITH_ANT_TWEAK_BAR
  TwTerminate();
#endif

  if (!ResourceManager::Destroy())
    return false;

  if (!DemoEngine::Destroy())
    return false;

  if (!Graphics::Destroy())
    return false;

  delete exch_null(_instance);
  return true;
}

//------------------------------------------------------------------------------
App& App::Instance()
{
  return *_instance;
}

//------------------------------------------------------------------------------
bool App::Init(HINSTANCE hinstance)
{
  if (_appRootFilename.empty())
  {
    MessageBoxA(0, "Unable to find app root", "Error", MB_ICONEXCLAMATION);
    return false;
  }

  _hinstance = hinstance;
  if (!ResourceManager::Create("resources.txt"))
    return false;

  if (!Graphics::Create(_hinstance))
    return false;

  int width = GetSystemMetrics(SM_CXFULLSCREEN);
  int height = GetSystemMetrics(SM_CYFULLSCREEN);

  GRAPHICS.CreateDefaultSwapChain(3 * width / 4, 3 * height / 4, DXGI_FORMAT_R8G8B8A8_UNORM, WndProc, hinstance);

  if (!DemoEngine::Create())
    return false;

  DEMO_ENGINE.RegisterFactory(ParticleTest::Name(), ParticleTest::Create);
  DEMO_ENGINE.RegisterFactory(SceneTest::Name(), SceneTest::Create);
  DEMO_ENGINE.RegisterFactory(GeneratorTest::Name(), GeneratorTest::Create);

#if WITH_ANT_TWEAK_BAR
  TwInit(TW_DIRECT3D11, GRAPHICS.Device());
  TwWindowSize(width, height);
#endif

  LoadSettings();

  if (!DEMO_ENGINE.Init("config/demo.pb", hinstance))
  {
    DeferredContext* ctx = GRAPHICS.CreateDeferredContext(true);
    ctx->SetSwapChain(GRAPHICS.DefaultSwapChain(), true);
    // If we get this far, then the graphics are initialized, so just spin displaying an
    // error message
    while (true)
    {
      if (GetAsyncKeyState(VK_ESCAPE) & (1 << 15))
        break;

      UpdateMessages();
      GRAPHICS.Present();
    }

    GRAPHICS.DestroyDeferredContext(ctx);

    return false;
  }


#if WITH_MUSIC
  FMOD_CHECKED(FMOD::System_Create(&_system));

  u32 version;
  FMOD_CHECKED(_system->getVersion(&version));

  if (version < FMOD_VERSION)
  {
    LOG_ERROR_LN("Error!  You are using an old version of FMOD %08x.  This program requires %08x\n", version, FMOD_VERSION);
    return false;
  }

  FMOD_CHECKED(_system->init(1, FMOD_INIT_NORMAL, 0));
  FMOD_CHECKED(_system->createStream("Never lose your smile.mp3", FMOD_HARDWARE | FMOD_LOOP_NORMAL | FMOD_2D, 0, &_sound));
#endif

  return true;
}

//------------------------------------------------------------------------------
void App::Run()
{
  MSG msg = {0};

  DEMO_ENGINE.Start();

#if WITH_MUSIC
  FMOD_RESULT result = _system->playSound(FMOD_CHANNEL_FREE, _sound, false, &_channel);
#endif

  while (WM_QUIT != msg.message)
  {
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) )
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
    else
    {

      DEMO_ENGINE.Tick();

#if WITH_UNPACKED_RESOUCES
      RESOURCE_MANAGER.Tick();
#endif

#if WITH_MUSIC
      _system->update();
#endif
#if WITH_ANT_TWEAK_BAR
      TwDraw();
#endif
      UpdateMessages();

      GRAPHICS.Present();
    }
  }

#if WITH_MUSIC
  FMOD_CHECKED(_sound->release());
  FMOD_CHECKED(_system->close());
  FMOD_CHECKED(_system->release());
#endif
}


//------------------------------------------------------------------------------
void App::LoadSettings()
{
  if (_appRootFilename.empty())
    return;
}


//------------------------------------------------------------------------------
void App::SaveSettings()
{
  if (_appRootFilename.empty())
    return;
}

//------------------------------------------------------------------------------
LRESULT App::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
#if WITH_ANT_TWEAK_BAR
  if (TwEventWin(hWnd, message, wParam, lParam))
  {
    return 0;
  }
#endif
  switch (message)
  {
  case WM_SIZE:
    //GRAPHICS.Resize(LOWORD(lParam), HIWORD(lParam));
    break;

  case WM_APP_CLOSE:
    DestroyWindow(hWnd);
    break;

  case WM_DESTROY:
    PostQuitMessage(0);
    break;

  case WM_LBUTTONUP:
  case WM_MBUTTONUP:
  case WM_RBUTTONUP:
    break;

  case WM_LBUTTONDOWN:
  case WM_MBUTTONDOWN:
  case WM_RBUTTONDOWN:
    break;

  case WM_MOUSEMOVE:
    break;

  case WM_MOUSEWHEEL:
    break;

  case WM_KEYUP:

    switch (wParam) {

      case 'S':
#if WITH_UNPACKED_RESOUCES
        if (GetKeyState(VK_CONTROL) & (1 << 15))
        {
          APP.SaveSettings();
          DEMO_ENGINE.SaveSettings();
          APP.AddMessage(MessageType::Info, "Settings saved");
        }
#endif
        break;

      case 'V':
        GRAPHICS.set_vsync(!GRAPHICS.vsync());
        return 0;

      case 'M': {
#if _DEBUG
        static _CrtMemState prevState;
        static bool firstTime = true;
        _CrtMemState curState;
        _CrtMemCheckpoint(&curState);

        if (!firstTime) {
          _CrtMemState stateDiff;
          OutputDebugStringA("*** MEM DIFF ***\n");
          _CrtMemDifference(&stateDiff, &prevState, &curState);
          _CrtMemDumpStatistics(&stateDiff);

        }
        firstTime = false;
        OutputDebugStringA("*** MEM STATS ***\n");
        _CrtMemDumpStatistics(&curState);
        prevState = curState;
#endif
        return 0;
      }

      case VK_ESCAPE:
        PostQuitMessage( 0 );
        return 0;
    
      case VK_SPACE:
        DEMO_ENGINE.SetPaused(!DEMO_ENGINE.Paused());
        return 0;

      case VK_PRIOR:
        DEMO_ENGINE.SetPos(DEMO_ENGINE.Pos() - seconds(1));
        return 0;

      case VK_NEXT:
        DEMO_ENGINE.SetPos(DEMO_ENGINE.Pos() + seconds(1));
        return 0;

      case VK_LEFT:
        DEMO_ENGINE.SetPos(DEMO_ENGINE.Pos() - milliseconds(100));
        return 0;

      case VK_RIGHT:
        DEMO_ENGINE.SetPos(DEMO_ENGINE.Pos() + milliseconds(100));
        return 0;

      case VK_HOME:
        DEMO_ENGINE.SetPos(seconds(0));
        return 0;

      default:
        break;
      }
    break;
  }

  if (DemoEngine::IsInitialized())
  {
    DEMO_ENGINE.WndProc(message, wParam, lParam);
  }

  return DefWindowProc( hWnd, message, wParam, lParam );
}

//------------------------------------------------------------------------------
void App::FindAppRoot()
{
  char starting_dir[MAX_PATH];
  _getcwd(starting_dir, MAX_PATH);

  // keep going up directory levels until we find "app.json", or we hit the bottom..
  char prev_dir[MAX_PATH], cur_dir[MAX_PATH];
  ZeroMemory(prev_dir, sizeof(prev_dir));

  while (true) {
    _getcwd(cur_dir, MAX_PATH);
    // check if we haven't moved
    if (!strcmp(cur_dir, prev_dir))
      break;

    memcpy(prev_dir, cur_dir, MAX_PATH);

#if WITH_UNPACKED_RESOUCES
    if (FileExists("app.pb"))
    {
      _appRoot = cur_dir;
      _appRootFilename = "app.json";
      return;
    }
#else
    if (FileExists("resources.dat"))
    {
      _appRoot = cur_dir;
      _appRootFilename = "app.json";
      return;
    }
#endif
    if (_chdir("..") == -1)
      break;
  }
  _appRoot = starting_dir;
}

//------------------------------------------------------------------------------
void App::AddMessage(MessageType type, const string& str)
{
  Message msg;
  msg.str = utf8_to_wide(str.c_str());
  if (type == MessageType::Info)
  {
    msg.r = msg.g = msg.b = 0.8f;
    msg.endTime = microsec_clock::local_time() + seconds(5);
  }
  else if (type == MessageType::Warning)
  {
    msg.r = msg.g = 0.9f; msg.b = 0;
    msg.endTime = microsec_clock::local_time() + seconds(10);
  }
  else
  {
    // error
    msg.r = 0.9f; msg.g = 0.2f; msg.b = 0.0f;
    msg.endTime = microsec_clock::local_time() + seconds(20);
  }

  _messages.push_back(msg);
}

//------------------------------------------------------------------------------
void App::UpdateMessages()
{
  ptime now = microsec_clock::local_time();

  float x = 300;
  float y = 0;

  for (auto it = _messages.begin(); it != _messages.end(); )
  {
    const Message& msg = *it;
    if (msg.endTime <= now)
    {
      // message has elapsed, so remove it
      it = _messages.erase(it);
    }
    else
    {
      ++it;
      y += 15;
      // blend out alpha over the last second
      time_duration left = msg.endTime - now;
      float a = 1;
      if (left < seconds(1))
      {
        a = clamp(0.0f, 1.0f, left.total_milliseconds() / 1000.0f);
      }
      GRAPHICS.AddText(msg.str.c_str(), "Arial", 15, x, y, ColorToU32(msg.r, msg.g, msg.b, a));
    }
  }
}
