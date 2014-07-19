#include "demo_bindings.hpp"
namespace demo
{
void BindPart(demo::Part* data, bool *dirty)
{
  struct Cfg
  {
    Part* data;
      bool *dirty;
  };

  static Cfg cfg;
  cfg.data = data;
  cfg.dirty = dirty;

#if WITH_ANT_TWEAK_BAR
  TwBar* bar = TwNewBar("demo.Part");
  // Add start
  TwAddVarCB(bar, "start", TW_TYPE_UINT32,
    [](const void* value, void* data) { Cfg* cfg = (Cfg*)data; cfg->data->set_start(*(uint32_t*)value); if (cfg->dirty) *cfg->dirty = true; },
    [](void* value, void* data) { Cfg* cfg = (Cfg*)data; *(uint32_t*)value = cfg->data->start(); }, (void*)&cfg, nullptr);

  // Add end
  TwAddVarCB(bar, "end", TW_TYPE_UINT32,
    [](const void* value, void* data) { Cfg* cfg = (Cfg*)data; cfg->data->set_end(*(uint32_t*)value); if (cfg->dirty) *cfg->dirty = true; },
    [](void* value, void* data) { Cfg* cfg = (Cfg*)data; *(uint32_t*)value = cfg->data->end(); }, (void*)&cfg, nullptr);

#endif
}
void BindConfig(demo::Config* data, bool *dirty)
{
  struct Cfg
  {
    Config* data;
      bool *dirty;
  };

  static Cfg cfg;
  cfg.data = data;
  cfg.dirty = dirty;

#if WITH_ANT_TWEAK_BAR
  TwBar* bar = TwNewBar("demo.Config");
#endif
}
}
