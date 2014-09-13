#include "animation.hpp"
#include "demo_engine.hpp"

using namespace boba;
using namespace bristol;

AnimationManager* AnimationManager::_instance;

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

//----------------------------------------------------------------------------------
float AnimationManager::Interpolate(ObjectHandle h, u32 time_ms)
{
  if (h.id() >= _floatAnimations.size())
    return 0;

  AnimDataFloat& data = _floatAnimations[h.id()];
  const FloatAnim& anim = data.anim;
  const vector<FloatKeyframe>& keyframes = anim.keyframe;

  if (keyframes.empty())
    return 0;

  if (time_ms <= keyframes.front().key.time || keyframes.size() == 1)
    return keyframes.front().key.value;

  if (time_ms >= keyframes.back().key.time)
    return keyframes.back().key.value;

  // NOTE: only catmull rom implemented for now

  // check if cached keyframe idx is valid
  u32 lastIdx = data.lastKeyframe;
  u32 maxIdx = (u32)keyframes.size() - 1;
  u32 curIdx;
  if (time_ms >= keyframes[lastIdx].key.time && time_ms <= keyframes[lastIdx+1].key.time)
  {
    curIdx = lastIdx;
  }
  else
  {
    // be sneaky, and check if we've advanced a single keyframe
    if (time_ms >= keyframes[lastIdx+1].key.time && time_ms <= keyframes[lastIdx+2].key.time)
    {
      curIdx = lastIdx+1;
    }
    else
    {
      // search for the keyframes
      for (curIdx = 0; curIdx < lastIdx; ++curIdx)
      {
        if (time_ms >= keyframes[curIdx].key.time && time_ms <= keyframes[curIdx+1].key.time)
          break;
      }
    }
  }

  // found the keyframes, so interpolate
  data.lastKeyframe = curIdx;

  const FloatKeyframe& p0 = keyframes[max(0, (s32)curIdx-1)];
  const FloatKeyframe& p1 = keyframes[max(0, (s32)curIdx+0)];
  const FloatKeyframe& p2 = keyframes[max(0, (s32)curIdx+1)];
  const FloatKeyframe& p3 = keyframes[min(maxIdx, lastIdx+2)];

  float t = (time_ms - p1.key.time) / (float)(p2.key.time - p1.key.time);
  return CatmulRom(p0.key.value, p1.key.value, p2.key.value, p3.key.value, t);
}

//----------------------------------------------------------------------------------
ObjectHandle AnimationManager::AddAnimation(const FloatAnim& animation, const ObjectHandle& prevHandle)
{
  if (prevHandle.IsValid())
  {
    // Overwrite the existing animation
    u32 id = prevHandle.id();
    if (id < _floatAnimations.size())
    {
      _floatAnimations[id] = {animation, 0};
      FindLastAnimation();
      return prevHandle;
    }
  }

  // Add new animation
  u32 id = (u32)_floatAnimations.size();
  _floatAnimations.push_back({animation, 0});
  FindLastAnimation();

  return ObjectHandle(ObjectHandle::kAnimation, id);
}

//----------------------------------------------------------------------------------
void AnimationManager::FindLastAnimation()
{
  s64 lastTime = 0;
  for (const AnimDataFloat& a : _floatAnimations)
  {
    for (const FloatKeyframe& k : a.anim.keyframe)
    {
      lastTime = max(lastTime, k.key.time);
    }
  }

  DEMO_ENGINE.SetDuration(TimeDuration::Milliseconds(lastTime));
}

//----------------------------------------------------------------------------------
AnimationManager& AnimationManager::Instance()
{
  return *_instance;
}

//----------------------------------------------------------------------------------
bool AnimationManager::Create()
{
  assert(!_instance);
  _instance = new AnimationManager();
  return true;
}

//----------------------------------------------------------------------------------
bool AnimationManager::Destroy()
{
  SAFE_DELETE(_instance);
  return true;
}
