#pragma once
#include "protocol/common_types.hpp"
#include "object_handle.hpp"

namespace boba
{
  //----------------------------------------------------------------------------------
  enum class AnimationType
  {
    Linear = 0,
    Bezier = 1,
    CatmullRom = 2,
  };

  //----------------------------------------------------------------------------------
  struct AnimationManager
  {

    float Interpolate(ObjectHandle h, u32 time_ms);
    ObjectHandle AddAnimation(const common::FloatAnim& animation, const ObjectHandle& prevHandle = ObjectHandle());

    static AnimationManager& Instance();
    static bool Create();
    static bool Destroy();

    template <typename T>
    struct AnimData
    {
      T anim;
      u32 lastKeyframe;
    };

    typedef AnimData<common::FloatAnim> AnimDataFloat;
    vector<AnimDataFloat> _floatAnimations;
    static AnimationManager* _instance;
  };

#define ANIMATION AnimationManager::Instance()

}