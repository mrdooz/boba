#pragma once


namespace boba
{
  struct Vector2;
  struct Vector3;
  struct Vector4;
  struct FloatKey;
  struct FloatKeyframe;
  struct AnimHeader;
  struct FloatAnim;
    
  struct FloatKey
  {
    s64 time;
    float value;
  };

  struct FloatKeyframe
  {
    FloatKey key;
    FloatKey cpIn;
    FloatKey cpOut;
  };

  struct AnimHeader
  {
    enum class AnimType
    {
      LINEAR = 0,
      BEZIER = 1,
      CATMULL_ROM = 2,
    };

    enum class LoopType
    {
      STOP = 0,
      LOOP = 1,
      LOOP_SMOOTH = 2,
    };

    AnimHeader::AnimType type = AnimType::CATMULL_ROM;
    AnimHeader::LoopType loop = LoopType::LOOP;
  };

  struct FloatAnim
  {
    AnimHeader header;
    vector<FloatKeyframe> keyframe;
  };

  
}