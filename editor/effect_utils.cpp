#include "effect_utils.hpp"

namespace editor
{
  namespace
  {
    //----------------------------------------------------------------------------------
    // find the keyframes that stradle the given time. return false is only a single
    // keyframe is found (ie before the first frame, or after the last one)
    bool FindKeyframePair(
        const vector<Vector3Keyframe>& keyframes,
        u32 time_ms,
        u32* idxLower,
        u32* idxUpper)
    {
      *idxLower = *idxUpper = 0xffffffff;

      if (time_ms <= keyframes.front().time)
      {
        *idxLower = 0;
        return false;
      }

      if (time_ms >= keyframes.back().time)
      {
        *idxLower = keyframes.size() - 1;
        return false;
      }

      // find upper idx
      int tmp = 0;
      while (time_ms > keyframes[tmp].time)
        ++tmp;

      *idxUpper = (u32)tmp;
      *idxLower = (u32)max(0, tmp-1);
      return true;
    }
  }

  //----------------------------------------------------------------------------------
  Vector3Keyframe* AddKeyframe(
      const time_duration& t,
      const Vector3f& value,
      bool forceAdd,
      Vector3Anim* anim)
  {
    vector<Vector3Keyframe>& keyframes = anim->keyframes;
    u32 time_ms = t.total_milliseconds();

    if (keyframes.empty())
    {
      keyframes.push_back({time_ms, value});
      return &keyframes.back();
    }

    if (time_ms <= keyframes.front().time)
    {
      keyframes.insert(keyframes.begin(), {time_ms, value});

      return &keyframes.front();
    }
    else if (time_ms >= keyframes.back().time)
    {
      keyframes.push_back({time_ms, value});
      return &keyframes.back();
    }

    // Insert the new keyframe after the lower index
    u32 idxLower, idxUpper;
    FindKeyframePair(keyframes, time_ms, &idxLower, &idxUpper);

    // check if the lower key has the same time as the current one, in which
    // case we'll just replace it
    if (!forceAdd && keyframes[idxLower].time == time_ms)
    {
      keyframes[idxLower].value = value;
      return &keyframes[idxLower];
    }
    else
    {
      u32 idx = idxUpper == 0xffffffff ? idxLower : idxUpper;
      auto it = keyframes.insert(keyframes.begin() + idx, {time_ms, value});
      return &(*it);
    }
  }

//----------------------------------------------------------------------------------
  Vector3f Interpolate(const Vector3Anim& anim, const time_duration& t)
  {
    return Interpolate(anim, t.total_milliseconds());
  }

//----------------------------------------------------------------------------------
  Vector3f Interpolate(const Vector3Anim& anim, u32 time_ms)
  {
    const vector<Vector3Keyframe>& keyframes = anim.keyframes;

    if (keyframes.empty())
      return Vector3f(0,0,0);

    u32 idxLower, idxUpper;
    if (!FindKeyframePair(keyframes, time_ms, &idxLower, &idxUpper))
    {
      return keyframes[idxLower].value;
    }

    const Vector3Keyframe& lower = keyframes[idxLower];
    const Vector3Keyframe& upper = keyframes[idxUpper];

    float t = (time_ms - lower.time) / (float)(upper.time - lower.time);

    if (anim.type == 0 || keyframes.size() == 2)
    {
      // linear interpolation
      return lower.value + t * (upper.value - lower.value);
    }
    else
    {
      // hermite splines
      Vector3f m0, m1;

      const Vector3f& p0 = lower.value;
      const Vector3f& p1 = upper.value;
      const Vector3f& p2 = keyframes[idxUpper+1].value;

      u32 t0 = lower.time;
      u32 t1 = upper.time;
      u32 t2 = keyframes[idxUpper+1].time;

      if (idxLower == 0)
      {
        m0 = Vector3f(0,0,0);
      }
      else
      {
        m0 = (p2 - p1) / (2.0f * (t2 - t1)) + (p1 - p0) / (2.0f * (t1 - t0));
      }

      if (idxUpper == keyframes.size() - 1)
      {
        m1 = Vector3f(0,0,0);
      }
    }

    return Vector3f(0,0,0);
  }
}
