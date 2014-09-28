#pragma once

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable: 4244 4267)
#endif

#include "effect_settings_particle.pb.h"
#include "proto_helpers.hpp"

#ifdef _WIN32
#pragma warning(pop)
#endif

namespace boba { namespace effect { namespace particle { 
  struct ParticleConfig;

  ParticleConfig FromProtocol(const ::protocol::effect::particle::ParticleConfig& p);
  void ToProtocol(const ParticleConfig& v, ::protocol::effect::particle::ParticleConfig* p);
} } } 