#pragma once
#pragma warning(push)
#pragma warning(disable: 4244 4267)
#include "particle.pb.h"
#include "generator.pb.h"
#pragma warning(pop)

namespace particle
{
void BindConfig(particle::Config* data, bool *dirty);
}
