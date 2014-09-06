#pragma once

#include "common_types.hpp"

namespace boba
{
  struct Plexus;
  struct TextPath;
  struct Displacement;
  struct NoiseEffector;
    
  struct Plexus
  {
    vector<TextPath> textPaths;
    vector<NoiseEffector> noiseEffectors;
  };

  struct TextPath
  {
    string text;
  };

  struct Displacement
  {
    FloatAnim x;
    FloatAnim y;
    FloatAnim z;
  };

  struct NoiseEffector
  {
    enum class ApplyTo
    {
      Position = 1,
      Scale = 2,
    };

    NoiseEffector::ApplyTo applyTo = ApplyTo::Position;
    Displacement displacement;
  };

  
}