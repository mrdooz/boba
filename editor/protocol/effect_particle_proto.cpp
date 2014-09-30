#include "effect_particle_proto.hpp"
#include "effect_particle_types.hpp"

namespace editor { namespace effect { namespace particle { 
  ParticleConfig FromProtocol(const ::protocol::effect::particle::ParticleConfig& p)
  {
    ParticleConfig res;
    if (p.has_num_particles())
      res.numParticles = p.num_particles();

    for (const auto& x : p.bb_col3f() )
      res.bbCol3F.push_back(x);

    for (const auto& x : p.bb_col4f() )
      res.bbCol4F.push_back(x);

    for (const auto& x : p.bb_dir3f() )
      res.bbDir3F.push_back(x);

    if (p.has_cc())
      res.cc = p.cc();

    return res;
  }

  void ToProtocol(const ParticleConfig& v, ::protocol::effect::particle::ParticleConfig* p)
  {
    p->set_num_particles(v.numParticles);
    for (const auto& x : v.bbCol3F)
      p->add_bb_col3f(x);
    for (const auto& x : v.bbCol4F)
      p->add_bb_col4f(x);
    for (const auto& x : v.bbDir3F)
      p->add_bb_dir3f(x);
    p->set_cc(v.cc);
  }

	
} } } 
