#pragma once
#include "effect_settings_particle.pb.h"
namespace protocol
{
namespace effect
{
namespace particle
{
void BindParticleConfig(protocol::effect::particle::ParticleConfig* data, bool *dirty);
}
}
}
