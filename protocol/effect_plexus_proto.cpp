#include "effect_plexus_proto.hpp"

namespace boba
{
  Plexus FromProtocol(const protocol::effect::plexus::Plexus& p)
  {
    Plexus res;
    for (const auto& x : p.text_paths() )
      res.textPaths.push_back(FromProtocol(x));

    for (const auto& x : p.noise_effectors() )
      res.noiseEffectors.push_back(FromProtocol(x));

    return res;
  }

  void ToProtocol(const Plexus& v, protocol::effect::plexus::Plexus* p)
  {
    for (const auto& x : v.textPaths)
      ToProtocol(x, p->add_text_paths());
    for (const auto& x : v.noiseEffectors)
      ToProtocol(x, p->add_noise_effectors());
  }

  TextPath FromProtocol(const protocol::effect::plexus::TextPath& p)
  {
    TextPath res;
    if (p.has_text())
      res.text = p.text();

    return res;
  }

  void ToProtocol(const TextPath& v, protocol::effect::plexus::TextPath* p)
  {
    p->set_text(v.text);
  }

  Displacement FromProtocol(const protocol::effect::plexus::Displacement& p)
  {
    Displacement res;
    if (p.has_x())
      res.x = FromProtocol(p.x());

    if (p.has_y())
      res.y = FromProtocol(p.y());

    if (p.has_z())
      res.z = FromProtocol(p.z());

    return res;
  }

  void ToProtocol(const Displacement& v, protocol::effect::plexus::Displacement* p)
  {
    ToProtocol(v.x, p->mutable_x());
    ToProtocol(v.y, p->mutable_y());
    ToProtocol(v.z, p->mutable_z());
  }

  NoiseEffector FromProtocol(const protocol::effect::plexus::NoiseEffector& p)
  {
    NoiseEffector res;
    res.applyTo = (NoiseEffector::ApplyTo)p.apply_to();

    if (p.has_displacement())
      res.displacement = FromProtocol(p.displacement());

    return res;
  }

  void ToProtocol(const NoiseEffector& v, protocol::effect::plexus::NoiseEffector* p)
  {
    ToProtocol(v.displacement, p->mutable_displacement());
  }

	
}
