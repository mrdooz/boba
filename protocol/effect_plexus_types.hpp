#pragma once

#include "common_types.hpp"

namespace boba { namespace effect { namespace plexus { 
  struct PlexusConfig;
  struct TextPathConfig;
  struct Displacement;
  struct NoiseEffectorConfig;
    
  struct PlexusConfig
  {

    vector<TextPathConfig> textPaths;
    vector<NoiseEffectorConfig> noiseEffectors;
  };
  struct TextPathConfig
  {

    string text;
  };
  struct Displacement
  {

    ::boba::common::FloatAnim x;
    ::boba::common::FloatAnim y;
    ::boba::common::FloatAnim z;
  };
  struct NoiseEffectorConfig
  {
    enum class ApplyTo
    {
      Position = 1,
      Scale = 2,
    };

    NoiseEffectorConfig::ApplyTo applyTo = ApplyTo::Position;
    Displacement displacement;
  };
  
} } } 