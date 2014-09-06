#pragma once


namespace editor
{
  struct Vector2;
  struct Vector3;
  struct Vector4;
  struct FloatKey;
  struct FloatKeyframe;
  struct FloatAnim;
  struct Vector3Keyframe;
  struct Vector3Anim;
    
  struct FloatKey
  {
    int64_t time;
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
    uint32_t type = 2;
    vector<FloatKeyframe> keyframe;
  };

  struct Vector3Keyframe
  {
    int32_t time;
    Vector3f value;
  };

  struct Vector3Anim
  {
    uint32_t type;
    vector<Vector3Keyframe> keyframe;
  };

  
}