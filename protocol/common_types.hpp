#pragma once


namespace boba
{
  struct Vector2;
  struct Vector3;
  struct Vector4;
  struct FloatKey;
  struct FloatKeyframe;
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

  struct FloatAnim
  {
    u32 type = 2;
    vector<FloatKeyframe> keyframe;
  };

  
}