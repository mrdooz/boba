#pragma once


namespace editor { namespace common { 
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
      CLAMP = 0,
      LOOP = 1,
      LOOP_SMOOTH = 2,
    };

    AnimHeader::AnimType type = AnimType::CATMULL_ROM;
    AnimHeader::LoopType loop = LoopType::LOOP;
  };
  struct FloatAnim
  {

    struct ValueFlagsF {
      enum Enum { UseMinValue = 1 << 0, UseMaxValue = 1 << 1, };
      struct Bits { u32 UseMinValue : 1; u32 UseMaxValue : 1; };
    };
    typedef Flags<ValueFlagsF> ValueFlags;
    AnimHeader header;
    vector<FloatKeyframe> keyframe;
    ValueFlags valueFlags;
    float minValue = 0;
    float maxValue = 10;
  };
  
} } 