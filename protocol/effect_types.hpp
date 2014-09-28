#pragma once

#include "effect_plexus_types.hpp"
#include "effect_generator_types.hpp"
#include "effect_particle_types.hpp"

namespace boba { namespace effect { 
  struct EffectSetting;
  struct EffectSettings;
    
  struct EffectSetting
  {
    enum class Type
    {
      Unknown = 0,
      Plexus = 1,
      Particle = 2,
      Generator = 3,
    };

    EffectSetting::Type type;
    u32 id;
    string name;
    u32 startTime;
    u32 endTime;
    ::boba::effect::plexus::PlexusConfig plexusConfig;
    ::boba::effect::generator::PlaneConfig generatorPlaneConfig;
    ::boba::effect::generator::SpikyConfig generatorSpikyConfig;
    ::boba::effect::particle::ParticleConfig particleConfig;
  };
  struct EffectSettings
  {

    vector<EffectSetting> effectSetting;
    string soundtrack;
  };
  
} } 