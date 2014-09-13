#include "timer.hpp"

using namespace boba;
using namespace bristol;

//------------------------------------------------------------------------------
Timer::Timer()
  : _running(false)
{
  Reset();
}

//------------------------------------------------------------------------------
void Timer::Reset()
{
  _curTime = _startTime = TimeStamp::Now();
  if (_cycle > 0)
    _endTime = _curTime + _cycle;
}

//------------------------------------------------------------------------------
void Timer::Start()
{
  if (_running)
    return;

  _running = true;
  Reset();
}

//------------------------------------------------------------------------------
void Timer::Stop()
{
  if (!_running)
    return;

  _running = false;
  _curTime = TimeStamp::Now();
}

//------------------------------------------------------------------------------
void Timer::SetElapsed(TimeDuration us)
{
  _startTime = _startTime + us;
  _curTime = _startTime;
}

//------------------------------------------------------------------------------
TimeDuration Timer::Elapsed(TimeDuration* delta)
{
  TimeStamp prev = _curTime;
  if (_running)
    _curTime = TimeStamp::Now();

  if (delta)
    *delta = _curTime - prev;

  // check if the timer has looped
  if (_cycle > 0 && _curTime > _endTime)
    Reset();

  return _curTime - _startTime;
}

//------------------------------------------------------------------------------
bool Timer::IsRunning() const
{
  return _running;
}

//------------------------------------------------------------------------------
void Timer::SetCycle(const TimeDuration& cycle)
{
  _cycle = cycle;
  _endTime = _startTime + _cycle;
}
