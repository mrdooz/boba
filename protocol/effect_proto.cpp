#include "effect_proto.hpp"

namespace boba
{
  EffectSetting FromProtocol(const protocol::effect::EffectSetting& p)
  {
    EffectSetting res;
    res.type = (EffectSetting::Type)p.type();

    if (p.has_effect_class())
      res.effectClass = p.effect_class();

    if (p.has_id())
      res.id = p.id();

    if (p.has_name())
      res.name = p.name();

    if (p.has_start_time())
      res.startTime = p.start_time();

    if (p.has_end_time())
      res.endTime = p.end_time();

    res.configMsg.resize(p.config_msg().size());
    memcpy(res.configMsg.data(), p.config_msg().data(), p.config_msg().size());

    if (p.has_config_file())
      res.configFile = p.config_file();

    return res;
  }

  void ToProtocol(const EffectSetting& v, protocol::effect::EffectSetting* p)
  {
    p->set_effect_class(v.effectClass);
    p->set_id(v.id);
    p->set_name(v.name);
    p->set_start_time(v.startTime);
    p->set_end_time(v.endTime);
    p->set_config_file(v.configFile);
  }

  EffectSettings FromProtocol(const protocol::effect::EffectSettings& p)
  {
    EffectSettings res;
    for (const auto& x : p.effect_setting() )
      res.effectSetting.push_back(FromProtocol(x));

    if (p.has_soundtrack())
      res.soundtrack = p.soundtrack();

    return res;
  }

  void ToProtocol(const EffectSettings& v, protocol::effect::EffectSettings* p)
  {
    for (const auto& x : v.effectSetting)
      ToProtocol(x, p->add_effect_setting());
    p->set_soundtrack(v.soundtrack);
  }

	
}
