#include "effect_plexus_proto.hpp"
#include "effect_plexus_types.hpp"
#include "../proto_helpers.hpp"

namespace editor { namespace effect { namespace plexus { 
  PlexusConfig FromProtocol(const ::protocol::effect::plexus::PlexusConfig& p)
  {
    PlexusConfig res;
    for (const auto& x : p.text_paths() )
      res.textPaths.push_back(::editor::effect::plexus::FromProtocol(x));

    for (const auto& x : p.noise_effectors() )
      res.noiseEffectors.push_back(::editor::effect::plexus::FromProtocol(x));

    return res;
  }

  void ToProtocol(const PlexusConfig& v, ::protocol::effect::plexus::PlexusConfig* p)
  {
    for (const auto& x : v.textPaths)
      ::editor::effect::plexus::ToProtocol(x, p->add_text_paths());
    for (const auto& x : v.noiseEffectors)
      ::editor::effect::plexus::ToProtocol(x, p->add_noise_effectors());
  }

  TextPathConfig FromProtocol(const ::protocol::effect::plexus::TextPathConfig& p)
  {
    TextPathConfig res;
    if (p.has_text())
      res.text = p.text();

    return res;
  }

  void ToProtocol(const TextPathConfig& v, ::protocol::effect::plexus::TextPathConfig* p)
  {
    p->set_text(v.text);
  }

  Displacement FromProtocol(const ::protocol::effect::plexus::Displacement& p)
  {
    Displacement res;
    if (p.has_x())
      res.x = ::editor::common::FromProtocol(p.x());

    if (p.has_y())
      res.y = ::editor::common::FromProtocol(p.y());

    if (p.has_z())
      res.z = ::editor::common::FromProtocol(p.z());

    return res;
  }

  void ToProtocol(const Displacement& v, ::protocol::effect::plexus::Displacement* p)
  {
    ::editor::common::ToProtocol(v.x, p->mutable_x());
    ::editor::common::ToProtocol(v.y, p->mutable_y());
    ::editor::common::ToProtocol(v.z, p->mutable_z());
  }

  NoiseEffectorConfig FromProtocol(const ::protocol::effect::plexus::NoiseEffectorConfig& p)
  {
    NoiseEffectorConfig res;
    res.applyTo = (NoiseEffectorConfig::ApplyTo)p.apply_to();

    if (p.has_displacement())
      res.displacement = ::editor::effect::plexus::FromProtocol(p.displacement());

    return res;
  }

  void ToProtocol(const NoiseEffectorConfig& v, ::protocol::effect::plexus::NoiseEffectorConfig* p)
  {
    ::editor::effect::plexus::ToProtocol(v.displacement, p->mutable_displacement());
  }

	
} } } 
