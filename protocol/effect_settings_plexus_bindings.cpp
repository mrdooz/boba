#include "effect_settings_plexus_bindings.hpp"
namespace effect
{
namespace protocol
{
namespace plexus
{
void BindPlexus(effect::protocol::plexus::Plexus* data, bool *dirty)
{
  struct Cfg
  {
    Plexus* data;
      bool *dirty;
  };

  static Cfg cfg;
  cfg.data = data;
  cfg.dirty = dirty;

#if WITH_ANT_TWEAK_BAR
  TwBar* bar = TwNewBar("effect.protocol.plexus.Plexus");
#endif
}
void BindTextPath(effect::protocol::plexus::TextPath* data, bool *dirty)
{
  struct Cfg
  {
    TextPath* data;
      bool *dirty;
  };

  static Cfg cfg;
  cfg.data = data;
  cfg.dirty = dirty;

#if WITH_ANT_TWEAK_BAR
  TwBar* bar = TwNewBar("effect.protocol.plexus.TextPath");
#endif
}
void BindNoiseEffector(effect::protocol::plexus::NoiseEffector* data, bool *dirty)
{
  struct Cfg
  {
    NoiseEffector* data;
      bool *dirty;
  };

  static Cfg cfg;
  cfg.data = data;
  cfg.dirty = dirty;

#if WITH_ANT_TWEAK_BAR
  TwBar* bar = TwNewBar("effect.protocol.plexus.NoiseEffector");
#endif
}
}
}
}
