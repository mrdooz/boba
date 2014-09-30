#include "common_proto.hpp"
#include "common_types.hpp"

namespace editor { namespace common { 
  FloatKey FromProtocol(const ::protocol::common::FloatKey& p)
  {
    FloatKey res;
    if (p.has_time())
      res.time = p.time();

    if (p.has_value())
      res.value = p.value();

    return res;
  }

  void ToProtocol(const FloatKey& v, ::protocol::common::FloatKey* p)
  {
    p->set_time(v.time);
    p->set_value(v.value);
  }

  FloatKeyframe FromProtocol(const ::protocol::common::FloatKeyframe& p)
  {
    FloatKeyframe res;
    if (p.has_key())
      res.key = ::editor::common::FromProtocol(p.key());

    if (p.has_cp_in())
      res.cpIn = ::editor::common::FromProtocol(p.cp_in());

    if (p.has_cp_out())
      res.cpOut = ::editor::common::FromProtocol(p.cp_out());

    return res;
  }

  void ToProtocol(const FloatKeyframe& v, ::protocol::common::FloatKeyframe* p)
  {
    ::editor::common::ToProtocol(v.key, p->mutable_key());
    ::editor::common::ToProtocol(v.cpIn, p->mutable_cp_in());
    ::editor::common::ToProtocol(v.cpOut, p->mutable_cp_out());
  }

  AnimHeader FromProtocol(const ::protocol::common::AnimHeader& p)
  {
    AnimHeader res;
    res.type = (AnimHeader::AnimType)p.type();

    res.loop = (AnimHeader::LoopType)p.loop();

    return res;
  }

  void ToProtocol(const AnimHeader& v, ::protocol::common::AnimHeader* p)
  {
  }

  FloatAnim FromProtocol(const ::protocol::common::FloatAnim& p)
  {
    FloatAnim res;
    if (p.has_header())
      res.header = ::editor::common::FromProtocol(p.header());

    for (const auto& x : p.keyframe() )
      res.keyframe.push_back(::editor::common::FromProtocol(x));


    if (p.has_min_value())
      res.minValue = p.min_value();

    if (p.has_max_value())
      res.maxValue = p.max_value();

    return res;
  }

  void ToProtocol(const FloatAnim& v, ::protocol::common::FloatAnim* p)
  {
    ::editor::common::ToProtocol(v.header, p->mutable_header());
    for (const auto& x : v.keyframe)
      ::editor::common::ToProtocol(x, p->add_keyframe());
    p->set_min_value(v.minValue);
    p->set_max_value(v.maxValue);
  }

	
} } 
