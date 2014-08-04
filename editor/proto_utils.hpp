#pragma once

#pragma warning(push)
#pragma warning(disable: 4244 4267)
#include "protocol/editor_settings.pb.h"
#include "protocol/effect_settings_plexus.pb.h"
#include "protocol/common.pb.h"
#pragma warning(pop)

#include "effect_utils.hpp"

namespace editor
{
  Color FromProtocol(const editor::Color4& col);
  Plexus FromProtocol(const effect::plexus::Plexus& plexus);
  TextPath FromProtocol(const effect::plexus::TextPath& textPath);
  NoiseEffector FromProtocol(const effect::plexus::NoiseEffector& effector);

  Vector3f FromProtocol(const common::Vector3& v);
  Vector3Keyframe FromProtocol(const common::Vector3Keyframe& x);
  Vector3Anim FromProtocol(const common::Vector3Anim& x);

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
}