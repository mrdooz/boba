#pragma once


namespace boba { namespace effect { namespace particle { 
  struct ParticleConfig;
    
  struct ParticleConfig
  {

    u32 numParticles;
    vector<float> bbCol3F;
    vector<float> bbCol4F;
    vector<float> bbDir3F;
    bool cc;
  };
  
} } } 