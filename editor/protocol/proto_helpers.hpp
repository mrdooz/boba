#pragma once

#include "editor_styles.pb.h"
#include "editor_types.hpp"
#include "effect_types.hpp"
#include "effect_plexus_types.hpp"

namespace editor
{
  inline Color FromProtocol(const protocol::common::Color4& col)
  {
    return Color(col.r(), col.g(), col.b(), col.a());
  }

  inline void ToProtocol(const Color& v, protocol::common::Color4* p)
  {
    p->set_r(v.r);
    p->set_g(v.g);
    p->set_b(v.b);
    p->set_a(v.a);
  }

  inline Vector3f FromProtocol(const protocol::common::Vector3& v)
  {
    return Vector3f(v.x(), v.y(), v.z());
  }

  inline void ToProtocol(const Vector3f& v, protocol::common::Vector3* p)
  {
    p->set_x(v.x);
    p->set_y(v.y);
    p->set_z(v.z);
  }

  inline Vector2f FromProtocol(const protocol::common::Vector2& v)
  {
    return Vector2f(v.x(), v.y());
  }

  inline void ToProtocol(const Vector2f& v, protocol::common::Vector2* p)
  {
    p->set_x(v.x);
    p->set_y(v.y);
  }

}
