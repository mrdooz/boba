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

  FloatKey FromProtocol(const protocol::common::FloatKey& p);
  void ToProtocol(const FloatKey& v, protocol::common::FloatKey* p);
  FloatKeyframe FromProtocol(const protocol::common::FloatKeyframe& p);
  void ToProtocol(const FloatKeyframe& v, protocol::common::FloatKeyframe* p);
  FloatAnim FromProtocol(const protocol::common::FloatAnim& p);
  void ToProtocol(const FloatAnim& v, protocol::common::FloatAnim* p);
}
