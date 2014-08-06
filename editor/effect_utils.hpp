#pragma once
#include "protocol/effects_proto.hpp"

namespace editor
{
  //----------------------------------------------------------------------------------
  template <typename T>
  bool LoadProto(const char* filename, T* out, bool textFormat = true)
  {
#pragma warning(suppress: 4996)
    FILE* f = fopen(filename, "rb");
    if (!f)
      return false;

    fseek(f, 0, 2);
    size_t s = ftell(f);
    fseek(f, 0, 0);
    std::string str;
    str.resize(s);
    fread((char*)str.c_str(), 1, s, f);
    fclose(f);

    return textFormat
        ? google::protobuf::TextFormat::ParseFromString(str, out)
        : out->ParseFromString(str);
  }

#if 0
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
#endif
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