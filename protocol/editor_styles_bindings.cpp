#include "editor_styles_bindings.hpp"
namespace editor
{
namespace protocol
{
void BindStyle(editor::protocol::Style* data, bool *dirty)
{
  struct Cfg
  {
    Style* data;
      bool *dirty;
  };

  static Cfg cfg;
  cfg.data = data;
  cfg.dirty = dirty;

#if WITH_ANT_TWEAK_BAR
  TwBar* bar = TwNewBar("editor.protocol.Style");
  // Add outline_thickness
  TwAddVarCB(bar, "outline_thickness", TW_TYPE_FLOAT,
    [](const void* value, void* data) { Cfg* cfg = (Cfg*)data; cfg->data->set_outline_thickness(*(float*)value); if (cfg->dirty) *cfg->dirty = true; },
    [](void* value, void* data) { Cfg* cfg = (Cfg*)data; *(float*)value = cfg->data->outline_thickness(); }, (void*)&cfg, nullptr);

  // Add font_style
  TwAddVarCB(bar, "font_style", TW_TYPE_UINT32,
    [](const void* value, void* data) { Cfg* cfg = (Cfg*)data; cfg->data->set_font_style(*(uint32_t*)value); if (cfg->dirty) *cfg->dirty = true; },
    [](void* value, void* data) { Cfg* cfg = (Cfg*)data; *(uint32_t*)value = cfg->data->font_style(); }, (void*)&cfg, nullptr);

#endif
}
void BindStyles(editor::protocol::Styles* data, bool *dirty)
{
  struct Cfg
  {
    Styles* data;
      bool *dirty;
  };

  static Cfg cfg;
  cfg.data = data;
  cfg.dirty = dirty;

#if WITH_ANT_TWEAK_BAR
  TwBar* bar = TwNewBar("editor.protocol.Styles");
#endif
}
}
}
