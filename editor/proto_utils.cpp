#include "proto_utils.hpp"

namespace editor
{
  //----------------------------------------------------------------------------------
  Color FromProtocol(const editor::Color4& col)
  {
    return Color(col.r(), col.g(), col.b(), col.a());
  }

  //----------------------------------------------------------------------------------
  Vector3f FromProtocol(const common::Vector3& v)
  {
    return Vector3f(v.x(), v.y(), v.z());
  }

  //----------------------------------------------------------------------------------
  Vector3Keyframe FromProtocol(const common::Vector3Keyframe& x)
  {
    return Vector3Keyframe(x.time(), FromProtocol(x.value()));
  }

  //----------------------------------------------------------------------------------
  Vector3Anim FromProtocol(const common::Vector3Anim& x)
  {
    Vector3Anim res;
    res.type = x.type();
    res.keyframes = FromProtocolRepeated<Vector3Keyframe>(x.keyframes());
    return res;
  }

  //----------------------------------------------------------------------------------
  TextPath FromProtocol(const effect::plexus::TextPath& textPath)
  {
    TextPath res;
    res.text = textPath.text();
    return res;
  }

  //----------------------------------------------------------------------------------
  NoiseEffector FromProtocol(const effect::plexus::NoiseEffector& effector)
  {
    NoiseEffector res;
    res.applyTo = (NoiseEffector::ApplyTo)effector.apply_to();
    if (effector.has_displacement())
    {
      res.displacement = FromProtocol(effector.displacement());
    }

    return res;
  }

  //----------------------------------------------------------------------------------
  Plexus FromProtocol(const effect::plexus::Plexus& plexus)
  {
    Plexus res;
    res.textPaths = FromProtocolRepeated<TextPath>(plexus.text_path());
    res.noiseEffectors = FromProtocolRepeated<NoiseEffector>(plexus.noise_effector());
    return res;
  }
}
