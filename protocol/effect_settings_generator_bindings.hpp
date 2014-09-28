#pragma once
#include "effect_settings_generator.pb.h"
namespace protocol
{
namespace effect
{
namespace generator
{
void BindSpikyConfig(protocol::effect::generator::SpikyConfig* data, bool *dirty);
void BindPlaneConfig(protocol::effect::generator::PlaneConfig* data, bool *dirty);
}
}
}
