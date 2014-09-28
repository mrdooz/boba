#pragma once

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable: 4244 4267)
#endif

#include "common_proto.hpp"
#include "effect_settings_generator.pb.h"
#include "proto_helpers.hpp"

#ifdef _WIN32
#pragma warning(pop)
#endif

namespace boba { namespace effect { namespace generator { 
  struct SpikyConfig;
  struct PlaneConfig;

  SpikyConfig FromProtocol(const ::protocol::effect::generator::SpikyConfig& p);
  void ToProtocol(const SpikyConfig& v, ::protocol::effect::generator::SpikyConfig* p);
  PlaneConfig FromProtocol(const ::protocol::effect::generator::PlaneConfig& p);
  void ToProtocol(const PlaneConfig& v, ::protocol::effect::generator::PlaneConfig* p);
} } } 