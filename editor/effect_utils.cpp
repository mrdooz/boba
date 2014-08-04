#include "effect_utils.hpp"

namespace editor
{
  Vector3f Interpolate(const Vector3Anim& anim, u32 time_ms)
  {
    const vector<Vector3Keyframe>& keyframes = anim.keyframes;

    if (keyframes.empty())
      return Vector3f(0,0,0);

    if (time_ms <= keyframes.front().time)
      return keyframes.front().value;

    if (time_ms >= keyframes.back().time)
      return keyframes.back().value;

    int idxUpper = 0;
    while (time_ms > keyframes[idxUpper].time)
      ++idxUpper;

    int idxLower = max(0, idxUpper-1);
    const Vector3Keyframe& kLower = keyframes[idxLower];
    const Vector3Keyframe& kUpper = keyframes[idxUpper];

    float t = (time_ms - kLower.time) / (float)(kUpper.time - kLower.time);
    return kLower.value + t * (kUpper.value - kLower.value);
  }
}
