#pragma once

#include "common_types.hpp"

namespace boba { namespace effect { namespace generator { 
  struct SpikyConfig;
  struct PlaneConfig;
    
  struct SpikyConfig
  {

    float radius;
    float height;
    u32 radialSegments;
    u32 heightSegments;
    Vector3 cameraPos;
    Vector3 cameraDir;
    Vector3 objT;
    Matrix objR;
  };
  struct PlaneConfig
  {

    float width;
    float height;
    u32 widthSegments;
    u32 heightSegments;
    Vector3 cameraPos;
    Vector3 cameraDir;
    Vector3 objT;
    Matrix objR;
    float tau = 0.5;
    float key = 0.18f;
    float ofs = 0.18f;
    float blurRadius = 10;
    float bloomThreshold = 0.8f;
    float bloomMultiplier = 1;
    bool transpose = true;
  };
  
} } } 