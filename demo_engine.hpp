#pragma once

#include "graphics.hpp"
#include "protocol/demo.pb.h"

namespace boba
{
  class Effect;

  typedef function<Effect*(const char*)> EffectFactory;

  class Timer
  {
  public:
    Timer();

    void Start();
    void Stop();
    void SetElapsed(time_duration us);
    time_duration Elapsed(time_duration* delta) const;
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

    void SetPos(time_duration pos);
    time_duration Pos();

    time_duration Duration() const;
    bool Tick();

    void WndProc(UINT message, WPARAM wParam, LPARAM lParam);

    static bool IsInitialized() { return _instance != nullptr; }

  private:
    DemoEngine();
    ~DemoEngine();
    static DemoEngine *_instance;

    void ReclassifyEffects();
    void KillEffects();

    Effect *FindEffectByName(const string &name);

    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    deque<Effect *> _activeEffects;
    deque<Effect *> _inactiveEffects;
    deque<Effect *> _expiredEffects;
    vector<Effect *> _effects;
    int _cur_effect;
    time_duration _duration;

    Timer _timer;

    demo::Config _config;
    unordered_map<string, EffectFactory> _effectFactories;
  };

#define DEMO_ENGINE DemoEngine::Instance()

}
