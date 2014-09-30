#pragma once

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable: 4244 4267)
#endif

#include "common_proto.hpp"
#include "effect_settings_plexus.pb.h"
#include "../proto_helpers.hpp"

#ifdef _WIN32
#pragma warning(pop)
#endif

namespace editor { namespace effect { namespace plexus { 
  struct PlexusConfig;
  struct TextPathConfig;
  struct Displacement;
  struct NoiseEffectorConfig;

  PlexusConfig FromProtocol(const ::protocol::effect::plexus::PlexusConfig& p);
  void ToProtocol(const PlexusConfig& v, ::protocol::effect::plexus::PlexusConfig* p);
  TextPathConfig FromProtocol(const ::protocol::effect::plexus::TextPathConfig& p);
  void ToProtocol(const TextPathConfig& v, ::protocol::effect::plexus::TextPathConfig* p);
  Displacement FromProtocol(const ::protocol::effect::plexus::Displacement& p);
  void ToProtocol(const Displacement& v, ::protocol::effect::plexus::Displacement* p);
  NoiseEffectorConfig FromProtocol(const ::protocol::effect::plexus::NoiseEffectorConfig& p);
  void ToProtocol(const NoiseEffectorConfig& v, ::protocol::effect::plexus::NoiseEffectorConfig* p);
} } } 