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
  Vector3f Interpolate(const Vector3Anim& anim, const time_duration& t);
  Vector3Keyframe* AddKeyframe(
      const time_duration& t,
      const Vector3f& value,
      bool forceAdd,
      Vector3Anim* anim);
}