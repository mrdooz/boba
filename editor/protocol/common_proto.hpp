#pragma once

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable: 4244 4267)
#endif

#include "common.pb.h"
#include "proto_helpers.hpp"

#ifdef _WIN32
#pragma warning(pop)
#endif

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

  FloatKey FromProtocol(const protocol::common::FloatKey& p);
  void ToProtocol(const FloatKey& v, protocol::common::FloatKey* p);
  FloatKeyframe FromProtocol(const protocol::common::FloatKeyframe& p);
  void ToProtocol(const FloatKeyframe& v, protocol::common::FloatKeyframe* p);
  FloatAnim FromProtocol(const protocol::common::FloatAnim& p);
  void ToProtocol(const FloatAnim& v, protocol::common::FloatAnim* p);
  Vector3Keyframe FromProtocol(const protocol::common::Vector3Keyframe& p);
  void ToProtocol(const Vector3Keyframe& v, protocol::common::Vector3Keyframe* p);
  Vector3Anim FromProtocol(const protocol::common::Vector3Anim& p);
  void ToProtocol(const Vector3Anim& v, protocol::common::Vector3Anim* p);
}
