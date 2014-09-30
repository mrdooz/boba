#include "effect_generator_proto.hpp"
#include "effect_generator_types.hpp"

namespace editor { namespace effect { namespace generator { 
  SpikyConfig FromProtocol(const ::protocol::effect::generator::SpikyConfig& p)
  {
    SpikyConfig res;
    if (p.has_radius())
      res.radius = p.radius();

    if (p.has_height())
      res.height = p.height();

    if (p.has_radial_segments())
      res.radialSegments = p.radial_segments();

    if (p.has_height_segments())
      res.heightSegments = p.height_segments();

    if (p.has_camera_pos())
      res.cameraPos = ::editor::common::FromProtocol(p.camera_pos());

    if (p.has_camera_dir())
      res.cameraDir = ::editor::common::FromProtocol(p.camera_dir());

    if (p.has_obj_t())
      res.objT = ::editor::common::FromProtocol(p.obj_t());

    if (p.has_obj_r())
      res.objR = ::editor::common::FromProtocol(p.obj_r());

    return res;
  }

  void ToProtocol(const SpikyConfig& v, ::protocol::effect::generator::SpikyConfig* p)
  {
    p->set_radius(v.radius);
    p->set_height(v.height);
    p->set_radial_segments(v.radialSegments);
    p->set_height_segments(v.heightSegments);
    ::editor::common::ToProtocol(v.cameraPos, p->mutable_camera_pos());
    ::editor::common::ToProtocol(v.cameraDir, p->mutable_camera_dir());
    ::editor::common::ToProtocol(v.objT, p->mutable_obj_t());
    ::editor::common::ToProtocol(v.objR, p->mutable_obj_r());
  }

  PlaneConfig FromProtocol(const ::protocol::effect::generator::PlaneConfig& p)
  {
    PlaneConfig res;
    if (p.has_width())
      res.width = p.width();

    if (p.has_height())
      res.height = p.height();

    if (p.has_width_segments())
      res.widthSegments = p.width_segments();

    if (p.has_height_segments())
      res.heightSegments = p.height_segments();

    if (p.has_camera_pos())
      res.cameraPos = ::editor::common::FromProtocol(p.camera_pos());

    if (p.has_camera_dir())
      res.cameraDir = ::editor::common::FromProtocol(p.camera_dir());

    if (p.has_obj_t())
      res.objT = ::editor::common::FromProtocol(p.obj_t());

    if (p.has_obj_r())
      res.objR = ::editor::common::FromProtocol(p.obj_r());

    if (p.has_tau())
      res.tau = p.tau();

    if (p.has_key())
      res.key = p.key();

    if (p.has_ofs())
      res.ofs = p.ofs();

    if (p.has_blur_radius())
      res.blurRadius = p.blur_radius();

    if (p.has_bloom_threshold())
      res.bloomThreshold = p.bloom_threshold();

    if (p.has_bloom_multiplier())
      res.bloomMultiplier = p.bloom_multiplier();

    if (p.has_transpose())
      res.transpose = p.transpose();

    return res;
  }

  void ToProtocol(const PlaneConfig& v, ::protocol::effect::generator::PlaneConfig* p)
  {
    p->set_width(v.width);
    p->set_height(v.height);
    p->set_width_segments(v.widthSegments);
    p->set_height_segments(v.heightSegments);
    ::editor::common::ToProtocol(v.cameraPos, p->mutable_camera_pos());
    ::editor::common::ToProtocol(v.cameraDir, p->mutable_camera_dir());
    ::editor::common::ToProtocol(v.objT, p->mutable_obj_t());
    ::editor::common::ToProtocol(v.objR, p->mutable_obj_r());
    p->set_tau(v.tau);
    p->set_key(v.key);
    p->set_ofs(v.ofs);
    p->set_blur_radius(v.blurRadius);
    p->set_bloom_threshold(v.bloomThreshold);
    p->set_bloom_multiplier(v.bloomMultiplier);
    p->set_transpose(v.transpose);
  }

	
} } } 
