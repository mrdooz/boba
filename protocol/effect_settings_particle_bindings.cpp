#include "effect_settings_particle_bindings.hpp"
namespace protocol
{
namespace effect
{
namespace particle
{
void BindParticleConfig(protocol::effect::particle::ParticleConfig* data, bool *dirty)
{
  struct Cfg
  {
    ParticleConfig* data;
      bool *dirty;
  };

  static Cfg cfg;
  cfg.data = data;
  cfg.dirty = dirty;

#if WITH_ANT_TWEAK_BAR
  TwBar* bar = TwNewBar("protocol.effect.particle.ParticleConfig");
  // Add num_particles
  TwAddVarCB(bar, "num_particles", TW_TYPE_UINT32,
    [](const void* value, void* data) { Cfg* cfg = (Cfg*)data; cfg->data->set_num_particles(*(uint32_t*)value); if (cfg->dirty) *cfg->dirty = true; },
    [](void* value, void* data) { Cfg* cfg = (Cfg*)data; *(uint32_t*)value = cfg->data->num_particles(); }, (void*)&cfg, nullptr);

  // Add bb_col3f
  TwAddVarCB(bar, "bb_col3f", TW_TYPE_COLOR3F,
    [](const void* value, void* data) { Cfg* cfg = (Cfg*)data; memcpy(cfg->data->mutable_bb_col3f()->mutable_data(), value, 3 * sizeof(float)); },
    [](void* value, void* data) { Cfg* cfg = (Cfg*)data; memcpy(value, cfg->data->bb_col3f().data(), 3 * sizeof(float)); }, (void*)&cfg, nullptr);

  // Add bb_col4f
  TwAddVarCB(bar, "bb_col4f", TW_TYPE_COLOR4F,
    [](const void* value, void* data) { Cfg* cfg = (Cfg*)data; memcpy(cfg->data->mutable_bb_col4f()->mutable_data(), value, 4 * sizeof(float)); },
    [](void* value, void* data) { Cfg* cfg = (Cfg*)data; memcpy(value, cfg->data->bb_col4f().data(), 4 * sizeof(float)); }, (void*)&cfg, nullptr);

  // Add bb_dir3f
  TwAddVarCB(bar, "bb_dir3f", TW_TYPE_DIR3F,
    [](const void* value, void* data) { Cfg* cfg = (Cfg*)data; memcpy(cfg->data->mutable_bb_dir3f()->mutable_data(), value, 3 * sizeof(float)); },
    [](void* value, void* data) { Cfg* cfg = (Cfg*)data; memcpy(value, cfg->data->bb_dir3f().data(), 3 * sizeof(float)); }, (void*)&cfg, nullptr);

  // Add cc
  TwAddVarCB(bar, "cc", TW_TYPE_BOOLCPP,
    [](const void* value, void* data) { Cfg* cfg = (Cfg*)data; cfg->data->set_cc(*(bool*)value); if (cfg->dirty) *cfg->dirty = true; },
    [](void* value, void* data) { Cfg* cfg = (Cfg*)data; *(bool*)value = cfg->data->cc(); }, (void*)&cfg, nullptr);

#endif
}
}
}
}
