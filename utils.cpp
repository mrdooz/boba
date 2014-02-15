#include "utils.hpp"

namespace boba
{
  //------------------------------------------------------------------------------
  string ToString(const char* format, ...)
  {
    va_list args;
    va_start(args, format);

    const int len = _vscprintf(format, args) + 1;

    char* buf = (char*)_alloca(len);
    vsprintf_s(buf, len, format, args);
    va_end(args);

    return string(buf);
  }

  //------------------------------------------------------------------------------
  CriticalSection::CriticalSection()
  {
    InitializeCriticalSection(&_cs);
  }

  //------------------------------------------------------------------------------
  CriticalSection::~CriticalSection()
  {
    DeleteCriticalSection(&_cs);
  }

  //------------------------------------------------------------------------------
  void CriticalSection::enter()
  {
    EnterCriticalSection(&_cs);
  }

  //------------------------------------------------------------------------------
  void CriticalSection::leave()
  {
    LeaveCriticalSection(&_cs);
  }

  //------------------------------------------------------------------------------
  ScopedCs::ScopedCs(CriticalSection &cs)
    : _cs(cs)
  {
    _cs.enter();
  }

  //------------------------------------------------------------------------------
  ScopedCs::~ScopedCs()
  {
    _cs.leave();
  }

  //------------------------------------------------------------------------------
  float GaussianRand(float mean, float variance)
  {
    // Generate a gaussian from the sum of uniformly distributed random numbers
    // (Central Limit Theorem)
    double sum = 0;
    const int numIters = 10;
    for (int i = 0; i < numIters; ++i)
    {
      sum += randf(-variance, variance);
    }
    return (float)(mean + sum / numIters);
  }

  //------------------------------------------------------------------------------
  void DebugOutput(const char* fmt, ...)
  {
    va_list args;
    va_start(args, fmt);

    const int len = _vscprintf(fmt, args) + 1;

    char* buf = (char*)_alloca(len);
    vsprintf_s(buf, len, fmt, args);
    OutputDebugStringA(buf);
    va_end(args);
  }
}

