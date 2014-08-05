#pragma once
#include "common.pb.h"
#include "effect_settings.pb.h"
#include "effect_settings_plexus.pb.h"
#include "../proto_utils.hpp"

namespace editor
{
  struct Vector2;
  struct Vector3;
  struct Vector4;
  struct Vector3Keyframe;
  struct Vector3Anim;
  struct Plexus;
  struct TextPath;
  struct NoiseEffector;

  Vector3Keyframe FromProtocol(const common::Vector3Keyframe& p);
  Vector3Anim FromProtocol(const common::Vector3Anim& p);
  Plexus FromProtocol(const effect::plexus::Plexus& p);
  TextPath FromProtocol(const effect::plexus::TextPath& p);
  NoiseEffector FromProtocol(const effect::plexus::NoiseEffector& p);
  Vector3f FromProtocol(const common::Vector3& p);

  template<typename T, typename U>
  vector<T> FromProtocolRepeated(const google::protobuf::RepeatedPtrField<U>& v)
  {
    vector<T> res;
    for (const auto& x : v)
    {
      res.push_back(FromProtocol(x));
    }
    return res;
  }

  struct Vector3Keyframe
  {
    uint32_t time;
    Vector3f value;
  };

  inline Vector3Keyframe FromProtocol(const common::Vector3Keyframe& p)
  {
    Vector3Keyframe res;
    res.time = p.time();
    res.value = FromProtocol(p.value());
    return res;
  }

  struct Vector3Anim
  {
    uint32_t type;
    vector<Vector3Keyframe> keyframes;
  };

  inline Vector3Anim FromProtocol(const common::Vector3Anim& p)
  {
    Vector3Anim res;
    res.type = p.type();
    res.keyframes = FromProtocolRepeated<Vector3Keyframe>(p.keyframes());
    return res;
  }

  struct Plexus
  {
    vector<TextPath> textPaths;
    vector<NoiseEffector> noiseEffectors;
  };

  inline Plexus FromProtocol(const effect::plexus::Plexus& p)
  {
    Plexus res;
    res.textPaths = FromProtocolRepeated<TextPath>(p.text_paths());
    res.noiseEffectors = FromProtocolRepeated<NoiseEffector>(p.noise_effectors());
    return res;
  }

  struct TextPath
  {
    string text;
  };

  inline TextPath FromProtocol(const effect::plexus::TextPath& p)
  {
    TextPath res;
    res.text = p.text();
    return res;
  }

  struct NoiseEffector
  {
    enum class ApplyTo
    {
      Position = 1,
      Scale = 2,
    };

    NoiseEffector::ApplyTo applyTo;
    Vector3Anim displacement;
  };

  inline NoiseEffector FromProtocol(const effect::plexus::NoiseEffector& p)
  {
    NoiseEffector res;
    res.applyTo = (NoiseEffector::ApplyTo)p.apply_to();
    res.displacement = FromProtocol(p.displacement());
    return res;
  }

	
}
