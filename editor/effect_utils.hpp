#pragma once

namespace editor
{
  struct Vector3Keyframe
  {
    Vector3Keyframe(u32 time, const Vector3f& v) : time_ms(time), value(v) {}
    u32 time_ms;
    Vector3f value;
  };

  struct Vector3Anim
  {
    int type;
    vector<Vector3Keyframe> keyframes;
  };

  struct TextPath
  {
    string text;
  };

  struct NoiseEffector
  {
    enum class ApplyTo {
      Position,
      Scale,
    };

    ApplyTo applyTo;
    Vector3Anim displacement;
  };

  struct Plexus
  {
    vector<TextPath> textPaths;
    vector<NoiseEffector> noiseEffectors;
  };

  struct Effect
  {
    enum class Type
    {
      Plexus,
    };

    union {
      Plexus* plexus;
    } data;

  };

  Vector3f Interpolate(const Vector3Anim& anim, u32 time_ms);
}