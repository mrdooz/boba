#include "editor_settings_bindings.hpp"
namespace editor
{
namespace protocol
{
void BindSettings(editor::protocol::Settings* data, bool *dirty)
{
  struct Cfg
  {
    Settings* data;
      bool *dirty;
  };

  static Cfg cfg;
  cfg.data = data;
  cfg.dirty = dirty;

#if WITH_ANT_TWEAK_BAR
  TwBar* bar = TwNewBar("editor.protocol.Settings");
  // Add ticker_height
  TwAddVarCB(bar, "ticker_height", TW_TYPE_UINT32,
    [](const void* value, void* data) { Cfg* cfg = (Cfg*)data; cfg->data->set_ticker_height(*(uint32_t*)value); if (cfg->dirty) *cfg->dirty = true; },
    [](void* value, void* data) { Cfg* cfg = (Cfg*)data; *(uint32_t*)value = cfg->data->ticker_height(); }, (void*)&cfg, nullptr);

  // Add ticker_interval
  TwAddVarCB(bar, "ticker_interval", TW_TYPE_UINT32,
    [](const void* value, void* data) { Cfg* cfg = (Cfg*)data; cfg->data->set_ticker_interval(*(uint32_t*)value); if (cfg->dirty) *cfg->dirty = true; },
    [](void* value, void* data) { Cfg* cfg = (Cfg*)data; *(uint32_t*)value = cfg->data->ticker_interval(); }, (void*)&cfg, nullptr);

  // Add ticks_per_interval
  TwAddVarCB(bar, "ticks_per_interval", TW_TYPE_UINT32,
    [](const void* value, void* data) { Cfg* cfg = (Cfg*)data; cfg->data->set_ticks_per_interval(*(uint32_t*)value); if (cfg->dirty) *cfg->dirty = true; },
    [](void* value, void* data) { Cfg* cfg = (Cfg*)data; *(uint32_t*)value = cfg->data->ticks_per_interval(); }, (void*)&cfg, nullptr);

  // Add effect_view_width
  TwAddVarCB(bar, "effect_view_width", TW_TYPE_UINT32,
    [](const void* value, void* data) { Cfg* cfg = (Cfg*)data; cfg->data->set_effect_view_width(*(uint32_t*)value); if (cfg->dirty) *cfg->dirty = true; },
    [](void* value, void* data) { Cfg* cfg = (Cfg*)data; *(uint32_t*)value = cfg->data->effect_view_width(); }, (void*)&cfg, nullptr);

  // Add effect_row_height
  TwAddVarCB(bar, "effect_row_height", TW_TYPE_UINT32,
    [](const void* value, void* data) { Cfg* cfg = (Cfg*)data; cfg->data->set_effect_row_height(*(uint32_t*)value); if (cfg->dirty) *cfg->dirty = true; },
    [](void* value, void* data) { Cfg* cfg = (Cfg*)data; *(uint32_t*)value = cfg->data->effect_row_height(); }, (void*)&cfg, nullptr);

  // Add status_bar_height
  TwAddVarCB(bar, "status_bar_height", TW_TYPE_UINT32,
    [](const void* value, void* data) { Cfg* cfg = (Cfg*)data; cfg->data->set_status_bar_height(*(uint32_t*)value); if (cfg->dirty) *cfg->dirty = true; },
    [](void* value, void* data) { Cfg* cfg = (Cfg*)data; *(uint32_t*)value = cfg->data->status_bar_height(); }, (void*)&cfg, nullptr);

  // Add effect_height
  TwAddVarCB(bar, "effect_height", TW_TYPE_UINT32,
    [](const void* value, void* data) { Cfg* cfg = (Cfg*)data; cfg->data->set_effect_height(*(uint32_t*)value); if (cfg->dirty) *cfg->dirty = true; },
    [](void* value, void* data) { Cfg* cfg = (Cfg*)data; *(uint32_t*)value = cfg->data->effect_height(); }, (void*)&cfg, nullptr);

  // Add resize_handle
  TwAddVarCB(bar, "resize_handle", TW_TYPE_UINT32,
    [](const void* value, void* data) { Cfg* cfg = (Cfg*)data; cfg->data->set_resize_handle(*(uint32_t*)value); if (cfg->dirty) *cfg->dirty = true; },
    [](void* value, void* data) { Cfg* cfg = (Cfg*)data; *(uint32_t*)value = cfg->data->resize_handle(); }, (void*)&cfg, nullptr);

  // Add timeline_zoom_min
  TwAddVarCB(bar, "timeline_zoom_min", TW_TYPE_UINT32,
    [](const void* value, void* data) { Cfg* cfg = (Cfg*)data; cfg->data->set_timeline_zoom_min(*(uint32_t*)value); if (cfg->dirty) *cfg->dirty = true; },
    [](void* value, void* data) { Cfg* cfg = (Cfg*)data; *(uint32_t*)value = cfg->data->timeline_zoom_min(); }, (void*)&cfg, nullptr);

  // Add timeline_zoom_max
  TwAddVarCB(bar, "timeline_zoom_max", TW_TYPE_UINT32,
    [](const void* value, void* data) { Cfg* cfg = (Cfg*)data; cfg->data->set_timeline_zoom_max(*(uint32_t*)value); if (cfg->dirty) *cfg->dirty = true; },
    [](void* value, void* data) { Cfg* cfg = (Cfg*)data; *(uint32_t*)value = cfg->data->timeline_zoom_max(); }, (void*)&cfg, nullptr);

  // Add timeline_zoom_default
  TwAddVarCB(bar, "timeline_zoom_default", TW_TYPE_UINT32,
    [](const void* value, void* data) { Cfg* cfg = (Cfg*)data; cfg->data->set_timeline_zoom_default(*(uint32_t*)value); if (cfg->dirty) *cfg->dirty = true; },
    [](void* value, void* data) { Cfg* cfg = (Cfg*)data; *(uint32_t*)value = cfg->data->timeline_zoom_default(); }, (void*)&cfg, nullptr);

#endif
}
}
}
