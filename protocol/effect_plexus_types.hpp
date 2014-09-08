#pragma once

#include "common_types.hpp"

namespace boba
{
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
    FloatAnim x;
    FloatAnim y;
    FloatAnim z;
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

  
}