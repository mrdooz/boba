#include "demo_engine.hpp"
#include "effect.hpp"
#include "file_utils.hpp"
#include "graphics.hpp"
#include "utils.hpp"
#include "proto_helpers.hpp"
#include "resource_manager.hpp"

using namespace boba;

//------------------------------------------------------------------------------
float TimeDurationToFloat(time_duration t)
{
  return t.total_microseconds() / 1e6f;
}

//------------------------------------------------------------------------------
Timer::Timer()
  : _running(false)
  , _startTime(0)
  , _curTime(0)
{
  QueryPerformanceFrequency((LARGE_INTEGER *)&_frequency);
}

//------------------------------------------------------------------------------
void Timer::Start()
{
  if (_running)
    return;

  _running = true;
  QueryPerformanceCounter((LARGE_INTEGER*)&_startTime);
}

//------------------------------------------------------------------------------
void Timer::Stop()
{
  if (!_running)
    return;

  _running = false;
  QueryPerformanceCounter((LARGE_INTEGER*)&_curTime);
}

//------------------------------------------------------------------------------
void Timer::SetElapsed(time_duration us)
{
  s64 ticks = us.total_microseconds() * _frequency / 1000000;
  _startTime = max((s64)0, _curTime - ticks);
}

//------------------------------------------------------------------------------
time_duration Timer::Elapsed(time_duration* delta) const
{
  s64 prev = _curTime;
  if (_running)
    QueryPerformanceCounter((LARGE_INTEGER*)&_curTime);

  if (delta)
    *delta = microseconds(1000000 * (_curTime - prev) / _frequency);

  // return elapsed time in us
  s64 elapsed = _curTime - _startTime;
  return microseconds(1000000 * elapsed / _frequency);
}

//------------------------------------------------------------------------------
bool Timer::IsRunning() const
{
  return _running;
}

//------------------------------------------------------------------------------
namespace
{
  Effect* Transfer(deque<Effect*>& src, deque<Effect*>& dst)
  {
    Effect* e = src.front();
    src.pop_front();
    dst.push_back(e);
    return e;
  }
}

//------------------------------------------------------------------------------
DemoEngine *DemoEngine::_instance = NULL;

//------------------------------------------------------------------------------
DemoEngine& DemoEngine::Instance()
{
  assert(_instance);
  return *_instance;
}

//------------------------------------------------------------------------------
DemoEngine::DemoEngine() 
  : _cur_effect(0)
  , _duration(seconds(180))
{
}

//------------------------------------------------------------------------------
DemoEngine::~DemoEngine()
{
  KillEffects();
  SeqDelete(&_effects);
  _activeEffects.clear();
  _inactiveEffects.clear();
  _expiredEffects.clear();
}

//------------------------------------------------------------------------------
void DemoEngine::KillEffects()
{
}

//------------------------------------------------------------------------------
bool DemoEngine::Create()
{
  assert(!_instance);
  _instance = new DemoEngine;
  return true;
}

//------------------------------------------------------------------------------
bool DemoEngine::Start()
{
  _timer.Start();
  return true;
}

//------------------------------------------------------------------------------
void DemoEngine::SetPaused(bool pause)
{
  if (pause)
    _timer.Stop();
  else
    _timer.Start();
}

//------------------------------------------------------------------------------
bool DemoEngine::Paused() const
{
  return !_timer.IsRunning();
}

//------------------------------------------------------------------------------
void DemoEngine::SetPos(time_duration pos)
{
  _timer.SetElapsed(pos);
  ReclassifyEffects();
}

//------------------------------------------------------------------------------
time_duration DemoEngine::Pos()
{
  return _timer.Elapsed(nullptr);
}

//------------------------------------------------------------------------------
time_duration DemoEngine::Duration() const
{
  return _duration;
}

//------------------------------------------------------------------------------
void DemoEngine::ReclassifyEffects()
{
  time_duration currentTime = _timer.Elapsed(nullptr);

  sort(_effects.begin(), _effects.end(),
      [](const Effect *a, const Effect *b) { return a->StartTime() < b->StartTime(); });

  _expiredEffects.clear();
  _inactiveEffects.clear();
  _activeEffects.clear();

  for (auto& effect : _effects)
  {
    bool started = currentTime >= effect->StartTime();
    bool ended = currentTime > effect->EndTime();
    effect->SetRunning(started && !ended);
    if (!started)
      _inactiveEffects.push_back(effect);
    else if (ended)
      _expiredEffects.push_back(effect);
    else
      _activeEffects.push_back(effect);
  }
}

//------------------------------------------------------------------------------
bool DemoEngine::Tick()
{
  time_duration delta, current = _timer.Elapsed(&delta);

  // check for any effects that have ended
  while (!_activeEffects.empty() && _activeEffects.front()->EndTime() <= current)
  {
    Effect* e = Transfer(_activeEffects, _expiredEffects);
    e->SetRunning(false);
  }

  // check if any effect start now
  unordered_set<Effect*> firstTimers;
  while (!_inactiveEffects.empty() && _inactiveEffects.front()->StartTime() <= current)
  {
    Effect* e = Transfer(_inactiveEffects, _activeEffects);
    firstTimers.insert(e);
    e->SetRunning(true);
  }

  bool paused = !_timer.IsRunning();

  // calc the number of ticks to step
  const int ticksPerSecond = 100;
  const float numTicks = TimeDurationToFloat(delta) * ticksPerSecond;
  const int intNumTicks = (int)numTicks;
  const float fracNumTicks = numTicks - intNumTicks;

  Effect::UpdateState curState;
  curState.globalTime = current;
  curState.delta = delta;
  curState.paused = paused;
  curState.frequency = ticksPerSecond;
  curState.numTicks = intNumTicks;
  curState.ticksFraction = fracNumTicks;

  Effect::UpdateState initialState;
  initialState.globalTime = current;
  initialState.localTime = seconds(0);
  initialState.delta = seconds(0);
  initialState.paused = paused;
  initialState.frequency = 100;
  initialState.numTicks = 1;
  initialState.ticksFraction = 0;

  // tick the active effects
  for (auto& effect : _activeEffects)
  {
    if (firstTimers.find(effect) != firstTimers.end())
    {
      effect->SetStartTime(current);
      effect->Update(initialState);
    }
    else
    {
      curState.localTime = current - effect->StartTime();
      effect->Update(curState);
    }

    effect->Render();
  }


  return true;
}

//------------------------------------------------------------------------------
bool DemoEngine::Destroy()
{
  delete exch_null(_instance);
  return true;
}

//------------------------------------------------------------------------------
Effect *DemoEngine::FindEffectByName(const string &name)
{
  auto it = find_if(_effects.begin(), _effects.end(),
      [&](const Effect *e) { return e->Name() == name; });
  return it == end(_effects) ? nullptr : *it;
}

//------------------------------------------------------------------------------
void DemoEngine::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
  for (size_t i = 0; i < _activeEffects.size(); ++i)
  {
    _activeEffects[i]->WndProc(message, wParam, lParam);
  }
}

//------------------------------------------------------------------------------
bool DemoEngine::Init(const char* config, HINSTANCE instance)
{
  if (!LoadProto(config, &_config))
    return false;


  for (const demo::Part& part : _config.part())
  {
    // Look up the factory
    auto factoryIt = _effectFactories.find(part.effect_class());
    if (factoryIt != _effectFactories.end())
    {
      const char* name = part.name().c_str();

      EffectFactory factory = factoryIt->second;
      Effect* effect = factory(name);
      effect->SetDuration(milliseconds(part.start()), milliseconds(part.end()));
      if (!effect->Init(part.config().c_str()))
      {
        return false;
      }
      _effects.push_back(effect);
    }
    else
    {
      //LOG_WARNING_LN("Unknown effect class");
      return false;
    }
  }

  ReclassifyEffects();
  return true;
}

//------------------------------------------------------------------------------
void DemoEngine::RegisterFactory(const char* demoClass, const EffectFactory& factory)
{
  _effectFactories[demoClass] = factory;
}

//------------------------------------------------------------------------------
LRESULT DemoEngine::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  return DefWindowProc(hWnd, message, wParam, lParam);
}

//------------------------------------------------------------------------------
void DemoEngine::SaveSettings()
{
  for (Effect* effect : _effects)
  {
    effect->SaveSettings();
  }
}
