#pragma once

namespace boba
{
  //----------------------------------------------------------------------------------
  template <typename T>
  T Bezier(const T& p0, const T& p1, const T& p2, const T& p3, float t)
  {
    float t2 = t*t;
    float t3 = t2*t;

    float u = (1-t);
    float u2 = u*u;
    float u3 = u2*u;

    return u3 * p0 + 3 * u2 * t * p1 + 3 * u * t2 * p2 + t3 * p3;
  }

  //----------------------------------------------------------------------------------
  template <typename T>
  T CatmulRom(const T& p0, const T& p1, const T& p2, const T& p3, float t)
  {
    float t2 = t*t;
    float t3 = t2*t;

    return 0.5f * ((2.f*p1) + (-p0 + p2) * t + (2.f*p0 - 5.f*p1 + 4.f*p2 -p3) * t2 + (-p0 + 3.f*p1 - 3.f*p2 + p3) * t3);
  }

}