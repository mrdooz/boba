#pragma once
#include "effect_settings.pb.h"
namespace effect
{
namespace protocol
{
void BindEffectSetting(effect::protocol::EffectSetting* data, bool *dirty);
void BindEffectSettings(effect::protocol::EffectSettings* data, bool *dirty);
}
}
