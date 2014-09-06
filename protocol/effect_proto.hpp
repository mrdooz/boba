#pragma once

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable: 4244 4267)
#endif

#include "effect_settings.pb.h"
#include "proto_helpers.hpp"

#ifdef _WIN32
#pragma warning(pop)
#endif

namespace boba
{
  struct EffectSetting;
  struct EffectSettings;

  EffectSetting FromProtocol(const protocol::effect::EffectSetting& p);
  void ToProtocol(const EffectSetting& v, protocol::effect::EffectSetting* p);
  EffectSettings FromProtocol(const protocol::effect::EffectSettings& p);
  void ToProtocol(const EffectSettings& v, protocol::effect::EffectSettings* p);
}
