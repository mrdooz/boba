#pragma once

#include "object_handle.hpp"

namespace protocol
{
  namespace effect
  {
    class EffectSetting;
  }
}

namespace boba
{
  class DeferredContext;

  struct UpdateState
  {
    TimeDuration globalTime;
    TimeDuration localTime;
    TimeDuration delta;
    s64 frequency;
    s32 numTicks;
    float ticksFraction;
    bool paused;
  };

  class Effect
  {
  public:

    Effect(const string& name, u32 id);
    virtual ~Effect();
    virtual bool Show();
    virtual bool Hide();
    virtual bool Init(const protocol::effect::EffectSetting& config);
    virtual bool Update(const UpdateState& state);
    virtual bool SaveSettings();
    virtual bool Render();
    virtual bool Close();
    virtual bool Reset();
    virtual void ToProtocol(protocol::effect::EffectSetting* settings) const {}
    virtual void FromProtocol(const string& str) {}

    virtual void WndProc(UINT message, WPARAM wParam, LPARAM lParam);

    const string& Name() const { return _name; }
    TimeDuration StartTime() const { return _startTime; }
    TimeDuration EndTime() const { return _endTime; }
    bool Running() const;
    void SetRunning(bool b);
    void SetDuration(TimeDuration startTime, TimeDuration endTime);
    void SetStartTime(TimeDuration startTime);

    u32 GetId() const { return _id; }

  protected:

    string _name;
    u32 _id;

    int _mouse_horiz;
    int _mouse_vert;
    bool _mouse_lbutton;
    bool _mouse_rbutton;
    DWORD _mouse_pos_prev;
    int _keystate[256];

    TimeDuration _startTime, _endTime;
    bool _running;

    DeferredContext* _ctx;
    bool _first_tick;
  };
}
