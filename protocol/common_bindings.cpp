#include "common_bindings.hpp"
namespace common
{
void BindVector2(common::Vector2* data, bool *dirty)
{
  struct Cfg
  {
    Vector2* data;
      bool *dirty;
  };

  static Cfg cfg;
  cfg.data = data;
  cfg.dirty = dirty;

  TwBar* bar = TwNewBar("common.Vector2");
  // Add x
  TwAddVarCB(bar, "x", TW_TYPE_FLOAT,
    [](const void* value, void* data) { Cfg* cfg = (Cfg*)data; cfg->data->set_x(*(float*)value); if (cfg->dirty) *cfg->dirty = true; },
    [](void* value, void* data) { Cfg* cfg = (Cfg*)data; *(float*)value = cfg->data->x(); }, (void*)&cfg, nullptr);

  // Add y
  TwAddVarCB(bar, "y", TW_TYPE_FLOAT,
    [](const void* value, void* data) { Cfg* cfg = (Cfg*)data; cfg->data->set_y(*(float*)value); if (cfg->dirty) *cfg->dirty = true; },
    [](void* value, void* data) { Cfg* cfg = (Cfg*)data; *(float*)value = cfg->data->y(); }, (void*)&cfg, nullptr);

}
void BindVector3(common::Vector3* data, bool *dirty)
{
  struct Cfg
  {
    Vector3* data;
      bool *dirty;
  };

  static Cfg cfg;
  cfg.data = data;
  cfg.dirty = dirty;

  TwBar* bar = TwNewBar("common.Vector3");
  // Add x
  TwAddVarCB(bar, "x", TW_TYPE_FLOAT,
    [](const void* value, void* data) { Cfg* cfg = (Cfg*)data; cfg->data->set_x(*(float*)value); if (cfg->dirty) *cfg->dirty = true; },
    [](void* value, void* data) { Cfg* cfg = (Cfg*)data; *(float*)value = cfg->data->x(); }, (void*)&cfg, nullptr);

  // Add y
  TwAddVarCB(bar, "y", TW_TYPE_FLOAT,
    [](const void* value, void* data) { Cfg* cfg = (Cfg*)data; cfg->data->set_y(*(float*)value); if (cfg->dirty) *cfg->dirty = true; },
    [](void* value, void* data) { Cfg* cfg = (Cfg*)data; *(float*)value = cfg->data->y(); }, (void*)&cfg, nullptr);

  // Add z
  TwAddVarCB(bar, "z", TW_TYPE_FLOAT,
    [](const void* value, void* data) { Cfg* cfg = (Cfg*)data; cfg->data->set_z(*(float*)value); if (cfg->dirty) *cfg->dirty = true; },
    [](void* value, void* data) { Cfg* cfg = (Cfg*)data; *(float*)value = cfg->data->z(); }, (void*)&cfg, nullptr);

}
void BindVector4(common::Vector4* data, bool *dirty)
{
  struct Cfg
  {
    Vector4* data;
      bool *dirty;
  };

  static Cfg cfg;
  cfg.data = data;
  cfg.dirty = dirty;

  TwBar* bar = TwNewBar("common.Vector4");
  // Add x
  TwAddVarCB(bar, "x", TW_TYPE_FLOAT,
    [](const void* value, void* data) { Cfg* cfg = (Cfg*)data; cfg->data->set_x(*(float*)value); if (cfg->dirty) *cfg->dirty = true; },
    [](void* value, void* data) { Cfg* cfg = (Cfg*)data; *(float*)value = cfg->data->x(); }, (void*)&cfg, nullptr);

  // Add y
  TwAddVarCB(bar, "y", TW_TYPE_FLOAT,
    [](const void* value, void* data) { Cfg* cfg = (Cfg*)data; cfg->data->set_y(*(float*)value); if (cfg->dirty) *cfg->dirty = true; },
    [](void* value, void* data) { Cfg* cfg = (Cfg*)data; *(float*)value = cfg->data->y(); }, (void*)&cfg, nullptr);

  // Add z
  TwAddVarCB(bar, "z", TW_TYPE_FLOAT,
    [](const void* value, void* data) { Cfg* cfg = (Cfg*)data; cfg->data->set_z(*(float*)value); if (cfg->dirty) *cfg->dirty = true; },
    [](void* value, void* data) { Cfg* cfg = (Cfg*)data; *(float*)value = cfg->data->z(); }, (void*)&cfg, nullptr);

  // Add w
  TwAddVarCB(bar, "w", TW_TYPE_FLOAT,
    [](const void* value, void* data) { Cfg* cfg = (Cfg*)data; cfg->data->set_w(*(float*)value); if (cfg->dirty) *cfg->dirty = true; },
    [](void* value, void* data) { Cfg* cfg = (Cfg*)data; *(float*)value = cfg->data->w(); }, (void*)&cfg, nullptr);

}
void BindMatrix3x3(common::Matrix3x3* data, bool *dirty)
{
  struct Cfg
  {
    Matrix3x3* data;
      bool *dirty;
  };

  static Cfg cfg;
  cfg.data = data;
  cfg.dirty = dirty;

  TwBar* bar = TwNewBar("common.Matrix3x3");
}
void BindMatrix4x4(common::Matrix4x4* data, bool *dirty)
{
  struct Cfg
  {
    Matrix4x4* data;
      bool *dirty;
  };

  static Cfg cfg;
  cfg.data = data;
  cfg.dirty = dirty;

  TwBar* bar = TwNewBar("common.Matrix4x4");
}
}
