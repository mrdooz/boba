#pragma once

#include "effect_particle_types.hpp"
#include "effect_plexus_types.hpp"
#include "effect_generator_types.hpp"

namespace editor { namespace effect { 
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
    ::editor::effect::plexus::PlexusConfig plexusConfig;
    ::editor::effect::generator::PlaneConfig generatorPlaneConfig;
    ::editor::effect::generator::SpikyConfig generatorSpikyConfig;
    ::editor::effect::particle::ParticleConfig particleConfig;
  };
  struct EffectSettings
  {

    vector<EffectSetting> effectSetting;
    string soundtrack;
  };
  
} } 