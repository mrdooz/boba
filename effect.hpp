#pragma once

#include "graphics_object_handle.hpp"

namespace boba
{
  class DeferredContext;

  class Effect
  {
  public:

    struct UpdateState
    {
      time_duration globalTime;
      time_duration localTime;
      time_duration delta;
      s64 frequency;
      s32 numTicks;
      float ticksFraction;
      bool paused;
    };

    Effect(const string& name);
    virtual ~Effect();
    virtual bool Init();
    virtual bool Update(const UpdateState& state);
    virtual bool Render();
    virtual bool Close();
    virtual bool Reset();

    virtual void WndProc(UINT message, WPARAM wParam, LPARAM lParam);

    const string& Name() const { return _name; }
    time_duration StartTime() const { return _startTime; }
    time_duration EndTime() const { return _endTime; }
    bool Running() const;
    void SetRunning(bool b);
    void SetDuration(time_duration startTime, time_duration endTime);
    void SetStartTime(time_duration startTime);

  protected:

    string _name;

    int _mouse_horiz;
    int _mouse_vert;
    bool _mouse_lbutton;
    bool _mouse_rbutton;
    DWORD _mouse_pos_prev;
    int _keystate[256];

    time_duration _startTime, _endTime;
    bool _running;

    DeferredContext *_ctx;
    bool _first_tick;
  };
}
