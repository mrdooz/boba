#include "common_bindings.hpp"
namespace common
{
namespace protocol
{
void BindColor4(common::protocol::Color4* data, bool *dirty)
{
  struct Cfg
  {
    Color4* data;
      bool *dirty;
  };

  static Cfg cfg;
  cfg.data = data;
  cfg.dirty = dirty;

#if WITH_ANT_TWEAK_BAR
  TwBar* bar = TwNewBar("common.protocol.Color4");
  // Add r
  TwAddVarCB(bar, "r", TW_TYPE_UINT32,
    [](const void* value, void* data) { Cfg* cfg = (Cfg*)data; cfg->data->set_r(*(uint32_t*)value); if (cfg->dirty) *cfg->dirty = true; },
    [](void* value, void* data) { Cfg* cfg = (Cfg*)data; *(uint32_t*)value = cfg->data->r(); }, (void*)&cfg, nullptr);

  // Add g
  TwAddVarCB(bar, "g", TW_TYPE_UINT32,
    [](const void* value, void* data) { Cfg* cfg = (Cfg*)data; cfg->data->set_g(*(uint32_t*)value); if (cfg->dirty) *cfg->dirty = true; },
    [](void* value, void* data) { Cfg* cfg = (Cfg*)data; *(uint32_t*)value = cfg->data->g(); }, (void*)&cfg, nullptr);

  // Add b
  TwAddVarCB(bar, "b", TW_TYPE_UINT32,
    [](const void* value, void* data) { Cfg* cfg = (Cfg*)data; cfg->data->set_b(*(uint32_t*)value); if (cfg->dirty) *cfg->dirty = true; },
    [](void* value, void* data) { Cfg* cfg = (Cfg*)data; *(uint32_t*)value = cfg->data->b(); }, (void*)&cfg, nullptr);

  // Add a
  TwAddVarCB(bar, "a", TW_TYPE_UINT32,
    [](const void* value, void* data) { Cfg* cfg = (Cfg*)data; cfg->data->set_a(*(uint32_t*)value); if (cfg->dirty) *cfg->dirty = true; },
    [](void* value, void* data) { Cfg* cfg = (Cfg*)data; *(uint32_t*)value = cfg->data->a(); }, (void*)&cfg, nullptr);

#endif
}
void BindVector2(common::protocol::Vector2* data, bool *dirty)
{
  struct Cfg
  {
    Vector2* data;
      bool *dirty;
  };

  static Cfg cfg;
  cfg.data = data;
  cfg.dirty = dirty;

#if WITH_ANT_TWEAK_BAR
  TwBar* bar = TwNewBar("common.protocol.Vector2");
  // Add x
  TwAddVarCB(bar, "x", TW_TYPE_FLOAT,
    [](const void* value, void* data) { Cfg* cfg = (Cfg*)data; cfg->data->set_x(*(float*)value); if (cfg->dirty) *cfg->dirty = true; },
    [](void* value, void* data) { Cfg* cfg = (Cfg*)data; *(float*)value = cfg->data->x(); }, (void*)&cfg, nullptr);

  // Add y
  TwAddVarCB(bar, "y", TW_TYPE_FLOAT,
    [](const void* value, void* data) { Cfg* cfg = (Cfg*)data; cfg->data->set_y(*(float*)value); if (cfg->dirty) *cfg->dirty = true; },
    [](void* value, void* data) { Cfg* cfg = (Cfg*)data; *(float*)value = cfg->data->y(); }, (void*)&cfg, nullptr);

#endif
}
void BindVector3(common::protocol::Vector3* data, bool *dirty)
{
  struct Cfg
  {
    Vector3* data;
      bool *dirty;
  };

  static Cfg cfg;
  cfg.data = data;
  cfg.dirty = dirty;

#if WITH_ANT_TWEAK_BAR
  TwBar* bar = TwNewBar("common.protocol.Vector3");
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

#endif
}
void BindVector4(common::protocol::Vector4* data, bool *dirty)
{
  struct Cfg
  {
    Vector4* data;
      bool *dirty;
  };

  static Cfg cfg;
  cfg.data = data;
  cfg.dirty = dirty;

#if WITH_ANT_TWEAK_BAR
  TwBar* bar = TwNewBar("common.protocol.Vector4");
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

#endif
}
void BindMatrix3x3(common::protocol::Matrix3x3* data, bool *dirty)
{
  struct Cfg
  {
    Matrix3x3* data;
      bool *dirty;
  };

  static Cfg cfg;
  cfg.data = data;
  cfg.dirty = dirty;

#if WITH_ANT_TWEAK_BAR
  TwBar* bar = TwNewBar("common.protocol.Matrix3x3");
#endif
}
void BindMatrix4x4(common::protocol::Matrix4x4* data, bool *dirty)
{
  struct Cfg
  {
    Matrix4x4* data;
      bool *dirty;
  };

  static Cfg cfg;
  cfg.data = data;
  cfg.dirty = dirty;

#if WITH_ANT_TWEAK_BAR
  TwBar* bar = TwNewBar("common.protocol.Matrix4x4");
#endif
}
void BindVector3Keyframe(common::protocol::Vector3Keyframe* data, bool *dirty)
{
  struct Cfg
  {
    Vector3Keyframe* data;
      bool *dirty;
  };

  static Cfg cfg;
  cfg.data = data;
  cfg.dirty = dirty;

#if WITH_ANT_TWEAK_BAR
  TwBar* bar = TwNewBar("common.protocol.Vector3Keyframe");
  // Add time
  TwAddVarCB(bar, "time", TW_TYPE_UINT32,
    [](const void* value, void* data) { Cfg* cfg = (Cfg*)data; cfg->data->set_time(*(uint32_t*)value); if (cfg->dirty) *cfg->dirty = true; },
    [](void* value, void* data) { Cfg* cfg = (Cfg*)data; *(uint32_t*)value = cfg->data->time(); }, (void*)&cfg, nullptr);

#endif
}
void BindVector3Anim(common::protocol::Vector3Anim* data, bool *dirty)
{
  struct Cfg
  {
    Vector3Anim* data;
      bool *dirty;
  };

  static Cfg cfg;
  cfg.data = data;
  cfg.dirty = dirty;

#if WITH_ANT_TWEAK_BAR
  TwBar* bar = TwNewBar("common.protocol.Vector3Anim");
  // Add type
  TwAddVarCB(bar, "type", TW_TYPE_UINT32,
    [](const void* value, void* data) { Cfg* cfg = (Cfg*)data; cfg->data->set_type(*(uint32_t*)value); if (cfg->dirty) *cfg->dirty = true; },
    [](void* value, void* data) { Cfg* cfg = (Cfg*)data; *(uint32_t*)value = cfg->data->type(); }, (void*)&cfg, nullptr);

#endif
}
}
}
