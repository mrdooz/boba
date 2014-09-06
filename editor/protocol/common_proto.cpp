#include "common_proto.hpp"

namespace editor
{
  FloatKey FromProtocol(const protocol::common::FloatKey& p)
  {
    FloatKey res;
    if (p.has_time())
      res.time = p.time();

    if (p.has_value())
      res.value = p.value();

    return res;
  }

  void ToProtocol(const FloatKey& v, protocol::common::FloatKey* p)
  {
    p->set_time(v.time);
    p->set_value(v.value);
  }

  FloatKeyframe FromProtocol(const protocol::common::FloatKeyframe& p)
  {
    FloatKeyframe res;
    if (p.has_key())
      res.key = FromProtocol(p.key());

    if (p.has_cp_in())
      res.cpIn = FromProtocol(p.cp_in());

    if (p.has_cp_out())
      res.cpOut = FromProtocol(p.cp_out());

    return res;
  }

  void ToProtocol(const FloatKeyframe& v, protocol::common::FloatKeyframe* p)
  {
    ToProtocol(v.key, p->mutable_key());
    ToProtocol(v.cpIn, p->mutable_cp_in());
    ToProtocol(v.cpOut, p->mutable_cp_out());
  }

  FloatAnim FromProtocol(const protocol::common::FloatAnim& p)
  {
    FloatAnim res;
    if (p.has_type())
      res.type = p.type();

    for (const auto& x : p.keyframe() )
      res.keyframe.push_back(FromProtocol(x));

    return res;
  }

  void ToProtocol(const FloatAnim& v, protocol::common::FloatAnim* p)
  {
    p->set_type(v.type);
    for (const auto& x : v.keyframe)
      ToProtocol(x, p->add_keyframe());
  }

  Vector3Keyframe FromProtocol(const protocol::common::Vector3Keyframe& p)
  {
    Vector3Keyframe res;
    if (p.has_time())
      res.time = p.time();

    if (p.has_value())
      res.value = FromProtocol(p.value());

    return res;
  }

  void ToProtocol(const Vector3Keyframe& v, protocol::common::Vector3Keyframe* p)
  {
    p->set_time(v.time);
    ToProtocol(v.value, p->mutable_value());
  }

  Vector3Anim FromProtocol(const protocol::common::Vector3Anim& p)
  {
    Vector3Anim res;
    if (p.has_type())
      res.type = p.type();

    for (const auto& x : p.keyframe() )
      res.keyframe.push_back(FromProtocol(x));

    return res;
  }

  void ToProtocol(const Vector3Anim& v, protocol::common::Vector3Anim* p)
  {
    p->set_type(v.type);
    for (const auto& x : v.keyframe)
      ToProtocol(x, p->add_keyframe());
  }

	
}
