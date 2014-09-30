#include "effect_proto.hpp"
#include "effect_types.hpp"

namespace boba { namespace effect { 
  EffectSetting FromProtocol(const ::protocol::effect::EffectSetting& p)
  {
    EffectSetting res;
    res.type = (EffectSetting::Type)p.type();

    if (p.has_id())
      res.id = p.id();

    if (p.has_name())
      res.name = p.name();

    if (p.has_start_time())
      res.startTime = p.start_time();

    if (p.has_end_time())
      res.endTime = p.end_time();

    if (p.has_plexus_config())
      res.plexusConfig = ::boba::effect::plexus::FromProtocol(p.plexus_config());

    if (p.has_generator_plane_config())
      res.generatorPlaneConfig = ::boba::effect::generator::FromProtocol(p.generator_plane_config());

    if (p.has_generator_spiky_config())
      res.generatorSpikyConfig = ::boba::effect::generator::FromProtocol(p.generator_spiky_config());

    if (p.has_particle_config())
      res.particleConfig = ::boba::effect::particle::FromProtocol(p.particle_config());

    return res;
  }

  void ToProtocol(const EffectSetting& v, ::protocol::effect::EffectSetting* p)
  {
    p->set_id(v.id);
    p->set_name(v.name);
    p->set_start_time(v.startTime);
    p->set_end_time(v.endTime);
    ::boba::effect::plexus::ToProtocol(v.plexusConfig, p->mutable_plexus_config());
    ::boba::effect::generator::ToProtocol(v.generatorPlaneConfig, p->mutable_generator_plane_config());
    ::boba::effect::generator::ToProtocol(v.generatorSpikyConfig, p->mutable_generator_spiky_config());
    ::boba::effect::particle::ToProtocol(v.particleConfig, p->mutable_particle_config());
  }

  EffectSettings FromProtocol(const ::protocol::effect::EffectSettings& p)
  {
    EffectSettings res;
    for (const auto& x : p.effect_setting() )
      res.effectSetting.push_back(::boba::effect::FromProtocol(x));

    if (p.has_soundtrack())
      res.soundtrack = p.soundtrack();

    return res;
  }

  void ToProtocol(const EffectSettings& v, ::protocol::effect::EffectSettings* p)
  {
    for (const auto& x : v.effectSetting)
      ::boba::effect::ToProtocol(x, p->add_effect_setting());
    p->set_soundtrack(v.soundtrack);
  }

	
} } 
