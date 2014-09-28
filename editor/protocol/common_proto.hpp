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

namespace editor { namespace common { 
  struct FloatKey;
  struct FloatKeyframe;
  struct AnimHeader;
  struct FloatAnim;

  FloatKey FromProtocol(const ::protocol::common::FloatKey& p);
  void ToProtocol(const FloatKey& v, ::protocol::common::FloatKey* p);
  FloatKeyframe FromProtocol(const ::protocol::common::FloatKeyframe& p);
  void ToProtocol(const FloatKeyframe& v, ::protocol::common::FloatKeyframe* p);
  AnimHeader FromProtocol(const ::protocol::common::AnimHeader& p);
  void ToProtocol(const AnimHeader& v, ::protocol::common::AnimHeader* p);
  FloatAnim FromProtocol(const ::protocol::common::FloatAnim& p);
  void ToProtocol(const FloatAnim& v, ::protocol::common::FloatAnim* p);
} } 