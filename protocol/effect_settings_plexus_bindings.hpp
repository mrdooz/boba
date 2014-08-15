#pragma once
#include "effect_settings_plexus.pb.h"
namespace effect
{
namespace protocol
{
namespace plexus
{
void BindPlexus(effect::protocol::plexus::Plexus* data, bool *dirty);
void BindTextPath(effect::protocol::plexus::TextPath* data, bool *dirty);
void BindNoiseEffector(effect::protocol::plexus::NoiseEffector* data, bool *dirty);
}
}
}
