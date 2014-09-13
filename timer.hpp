#pragma once

namespace boba
{
  class Timer
  {
  public:
    Timer();

    void Start();
    void Stop();
    void SetElapsed(TimeDuration t);
    TimeDuration Elapsed(TimeDuration* delta);
    bool IsRunning() const;
    void Reset();
    void SetCycle(const TimeDuration& t);

  private:
    bool _running;
    TimeStamp _startTime;
    TimeDuration _cycle;
    TimeStamp _endTime;
    TimeStamp _curTime;
  };

}