#include "generator_bindings.hpp"
namespace generator
{
void BindSpiky(generator::Spiky* data, bool *dirty)
{
  struct Cfg
  {
    Spiky* data;
      bool *dirty;
  };

  static Cfg cfg;
  cfg.data = data;
  cfg.dirty = dirty;

  TwBar* bar = TwNewBar("generator.Spiky");
  // Add radius
  TwAddVarCB(bar, "radius", TW_TYPE_FLOAT,
    [](const void* value, void* data) { Cfg* cfg = (Cfg*)data; cfg->data->set_radius(*(float*)value); if (cfg->dirty) *cfg->dirty = true; },
    [](void* value, void* data) { Cfg* cfg = (Cfg*)data; *(float*)value = cfg->data->radius(); }, (void*)&cfg, nullptr);

  // Add height
  TwAddVarCB(bar, "height", TW_TYPE_FLOAT,
    [](const void* value, void* data) { Cfg* cfg = (Cfg*)data; cfg->data->set_height(*(float*)value); if (cfg->dirty) *cfg->dirty = true; },
    [](void* value, void* data) { Cfg* cfg = (Cfg*)data; *(float*)value = cfg->data->height(); }, (void*)&cfg, nullptr);

  // Add radial_segments
  TwAddVarCB(bar, "radial_segments", TW_TYPE_UINT32,
    [](const void* value, void* data) { Cfg* cfg = (Cfg*)data; cfg->data->set_radial_segments(*(uint32_t*)value); if (cfg->dirty) *cfg->dirty = true; },
    [](void* value, void* data) { Cfg* cfg = (Cfg*)data; *(uint32_t*)value = cfg->data->radial_segments(); }, (void*)&cfg, nullptr);

  // Add height_segments
  TwAddVarCB(bar, "height_segments", TW_TYPE_UINT32,
    [](const void* value, void* data) { Cfg* cfg = (Cfg*)data; cfg->data->set_height_segments(*(uint32_t*)value); if (cfg->dirty) *cfg->dirty = true; },
    [](void* value, void* data) { Cfg* cfg = (Cfg*)data; *(uint32_t*)value = cfg->data->height_segments(); }, (void*)&cfg, nullptr);

}
void BindPlane(generator::Plane* data, bool *dirty)
{
  struct Cfg
  {
    Plane* data;
      bool *dirty;
  };

  static Cfg cfg;
  cfg.data = data;
  cfg.dirty = dirty;

  TwBar* bar = TwNewBar("generator.Plane");
  // Add width
  TwAddVarCB(bar, "width", TW_TYPE_FLOAT,
    [](const void* value, void* data) { Cfg* cfg = (Cfg*)data; cfg->data->set_width(*(float*)value); if (cfg->dirty) *cfg->dirty = true; },
    [](void* value, void* data) { Cfg* cfg = (Cfg*)data; *(float*)value = cfg->data->width(); }, (void*)&cfg, nullptr);

  // Add height
  TwAddVarCB(bar, "height", TW_TYPE_FLOAT,
    [](const void* value, void* data) { Cfg* cfg = (Cfg*)data; cfg->data->set_height(*(float*)value); if (cfg->dirty) *cfg->dirty = true; },
    [](void* value, void* data) { Cfg* cfg = (Cfg*)data; *(float*)value = cfg->data->height(); }, (void*)&cfg, nullptr);

  // Add width_segments
  TwAddVarCB(bar, "width_segments", TW_TYPE_UINT32,
    [](const void* value, void* data) { Cfg* cfg = (Cfg*)data; cfg->data->set_width_segments(*(uint32_t*)value); if (cfg->dirty) *cfg->dirty = true; },
    [](void* value, void* data) { Cfg* cfg = (Cfg*)data; *(uint32_t*)value = cfg->data->width_segments(); }, (void*)&cfg, nullptr);

  // Add height_segments
  TwAddVarCB(bar, "height_segments", TW_TYPE_UINT32,
    [](const void* value, void* data) { Cfg* cfg = (Cfg*)data; cfg->data->set_height_segments(*(uint32_t*)value); if (cfg->dirty) *cfg->dirty = true; },
    [](void* value, void* data) { Cfg* cfg = (Cfg*)data; *(uint32_t*)value = cfg->data->height_segments(); }, (void*)&cfg, nullptr);

}
}
