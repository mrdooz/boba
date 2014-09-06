#pragma once

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable: 4244 4267)
#endif

#include "common_proto.hpp"
#include "effect_settings_plexus.pb.h"
#include "proto_helpers.hpp"

#ifdef _WIN32
#pragma warning(pop)
#endif

namespace editor
{
  struct Plexus;
  struct TextPath;
  struct Displacement;
  struct NoiseEffector;

  Plexus FromProtocol(const protocol::effect::plexus::Plexus& p);
  void ToProtocol(const Plexus& v, protocol::effect::plexus::Plexus* p);
  TextPath FromProtocol(const protocol::effect::plexus::TextPath& p);
  void ToProtocol(const TextPath& v, protocol::effect::plexus::TextPath* p);
  Displacement FromProtocol(const protocol::effect::plexus::Displacement& p);
  void ToProtocol(const Displacement& v, protocol::effect::plexus::Displacement* p);
  NoiseEffector FromProtocol(const protocol::effect::plexus::NoiseEffector& p);
  void ToProtocol(const NoiseEffector& v, protocol::effect::plexus::NoiseEffector* p);
}
