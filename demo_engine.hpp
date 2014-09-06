#pragma once

#include "graphics.hpp"
#pragma warning(push)
#pragma warning(disable: 4244 4267)
#include "protocol/effect_settings.pb.h"
#pragma warning(pop)

namespace boba
{
  class Effect;

  typedef function<Effect*(const char*, u32)> EffectFactory;

  class Timer
  {
  public:
    Timer();

    void Start();
    void Stop();
    void SetElapsed(TimeDuration us);
    TimeDuration Elapsed(TimeDuration* delta) const;
    bool IsRunning() const;

  private:
    bool _running;
    s64 _frequency;
    s64 _startTime;
    mutable s64 _curTime;
  };

  class DemoEngine
  {
  public:
    static DemoEngine &Instance();
    static bool Create();
    static bool Destroy();

    bool Init(const char* config, HINSTANCE instance);
    void RegisterFactory(const char* demoClass, const EffectFactory& factory);

    void SaveSettings();

    bool Start();
    void SetPaused(bool pause);
    bool Paused() const;

    void SetPos(TimeDuration pos);
    TimeDuration Pos();

    TimeDuration Duration() const;
    bool Tick();

    void WndProc(UINT message, WPARAM wParam, LPARAM lParam);

    static bool IsInitialized() { return _instance != nullptr; }

    void ProcessPayload(const void* payload, u32 size);
    void Connected();

  private:
    DemoEngine();
    ~DemoEngine();
    static DemoEngine* _instance;

    void ReclassifyEffects();
    void KillEffects();

    Effect* FindEffectByName(const string &name);

    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    deque<Effect*> _activeEffects;
    deque<Effect*> _inactiveEffects;
    deque<Effect*> _expiredEffects;
    vector<Effect*> _effects;
    int _cur_effect;
    TimeDuration _duration;

    Timer _timer;


    protocol::effect::EffectSettings _config;
    unordered_map<string, EffectFactory> _effectFactories;
    u32 _nextEffectId;
  };

#define DEMO_ENGINE DemoEngine::Instance()

}
