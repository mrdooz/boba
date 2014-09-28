#include "demo_engine.hpp"
#include "effect.hpp"
#include "graphics.hpp"
#include "proto_helpers.hpp"
#include "resource_manager.hpp"
#include "app.hpp"
#include "init_sequence.hpp"

#pragma warning(push)
#pragma warning(disable: 4244 4267)
#include "protocol/effect_settings.pb.h"
#pragma warning(pop)

using namespace boba;
using namespace bristol;

//------------------------------------------------------------------------------
float TimeDurationToFloat(TimeDuration t)
{
  return t.TotalMicroseconds() / 1e6f;
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
  , _duration(TimeDuration::Seconds(180))
  , _nextEffectId(1)
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
void DemoEngine::SetPos(TimeDuration pos)
{
  _timer.SetElapsed(pos);
  ReclassifyEffects();
}

//------------------------------------------------------------------------------
TimeDuration DemoEngine::Pos()
{
  return _timer.Elapsed(nullptr);
}

//------------------------------------------------------------------------------
TimeDuration DemoEngine::Duration() const
{
  return _duration;
}

//------------------------------------------------------------------------------
void DemoEngine::SetDuration(const TimeDuration& duration)
{
  _duration = duration;
  _timer.SetCycle(duration);
}

//------------------------------------------------------------------------------
void DemoEngine::ReclassifyEffects()
{
  TimeDuration currentTime = _timer.Elapsed(nullptr);

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
  TimeDuration delta, current;
  current = _timer.Elapsed(&delta);

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

  UpdateState curState;
  curState.globalTime = current;
  curState.delta = delta;
  curState.paused = paused;
  curState.frequency = ticksPerSecond;
  curState.numTicks = intNumTicks;
  curState.ticksFraction = fracNumTicks;

  UpdateState initialState;
  initialState.globalTime = current;
  initialState.localTime = TimeDuration::Seconds(0);
  initialState.delta = TimeDuration::Seconds(0);
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
  BEGIN_INIT_SEQUENCE();
  _configFile = config;

  INIT_FATAL(LoadProto(config, &_config));

  for (const protocol::effect::EffectSetting& effect : _config.effect_setting())
  {
    // Look up the factory
    effect::EffectSetting::Type type = (effect::EffectSetting::Type)effect.type();
    auto factoryIt = _effectFactories.find(type);
    if (factoryIt != _effectFactories.end())
    {
      const char* name = effect.name().c_str();
      EffectFactory factory = factoryIt->second;
      Effect* newEffect = factory(name, _nextEffectId++);
      newEffect->SetDuration(TimeDuration::Milliseconds(effect.start_time()), TimeDuration::Milliseconds(effect.end_time()));

      switch (type)
      {
        case effect::EffectSetting::Type::Generator: INIT(newEffect->Init(effect)); break;
        case effect::EffectSetting::Type::Particle: INIT(newEffect->Init(effect)); break;
        case effect::EffectSetting::Type::Plexus: INIT(newEffect->Init(effect)); break;
      }
      _effects.push_back(newEffect);
    }
    else
    {
      LOG_WARN("Unknown effect class");
      return false;
    }
  }

  ReclassifyEffects();
  END_INIT_SEQUENCE();
}

//------------------------------------------------------------------------------
void DemoEngine::RegisterFactory(effect::EffectSetting::Type type, const EffectFactory& factory)
{
  _effectFactories[type] = factory;
}

//------------------------------------------------------------------------------
LRESULT DemoEngine::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  return DefWindowProc(hWnd, message, wParam, lParam);
}

//------------------------------------------------------------------------------
void DemoEngine::SaveSettings()
{
  if (FILE* f = fopen("config/config.tmp", "wt"))
  {
    fprintf(f, "%s", _config.DebugString().c_str());
    fclose(f);
  }

//   for (Effect* effect : _effects)
//   {
//     effect->SaveSettings();
//   }
}

//------------------------------------------------------------------------------
void DemoEngine::Connected()
{
  // send current config to the editor
  for (const Effect* effect : _effects)
  {
    protocol::effect::EffectSetting* setting = _config.add_effect_setting();
    setting->set_id(effect->GetId());
    effect->ToProtocol(setting);
  }

  string str = _config.SerializeAsString();
  APP.SendWebsocketFrame((const u8*)str.data(), (int)str.size());
}

//------------------------------------------------------------------------------
void DemoEngine::ProcessPayload(const void* payload, u32 size)
{
  if (_config.ParseFromArray(payload, size))
  {
    for (const protocol::effect::EffectSetting& setting : _config.effect_setting())
    {
      u32 id = setting.id();

      if (setting.id() == 0)
      {
        // if the effect has id 0, it's a new effect
      }
      else
      {
        // find effect with the given id
        auto it = find_if(_effects.begin(), _effects.end(), [=](const Effect* e) { return e->GetId() == id; });
        if (it != _effects.end())
        {
          // TODO:
          //(*it)->FromProtocol(setting.config_msg());
        }
      }
    }

  }

}
