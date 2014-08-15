#include "effect_settings_bindings.hpp"
namespace effect
{
namespace protocol
{
void BindEffectSetting(effect::protocol::EffectSetting* data, bool *dirty)
{
  struct Cfg
  {
    EffectSetting* data;
      bool *dirty;
  };

  static Cfg cfg;
  cfg.data = data;
  cfg.dirty = dirty;

#if WITH_ANT_TWEAK_BAR
  TwBar* bar = TwNewBar("effect.protocol.EffectSetting");
#endif
}
void BindEffectSettings(effect::protocol::EffectSettings* data, bool *dirty)
{
  struct Cfg
  {
    EffectSettings* data;
      bool *dirty;
  };

  static Cfg cfg;
  cfg.data = data;
  cfg.dirty = dirty;

#if WITH_ANT_TWEAK_BAR
  TwBar* bar = TwNewBar("effect.protocol.EffectSettings");
#endif
}
}
}
