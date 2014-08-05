#pragma once

#pragma warning(push)
#pragma warning(disable: 4244 4267)
#include "protocol/editor_settings.pb.h"
#include "protocol/effect_settings_plexus.pb.h"
#include "protocol/common.pb.h"
#pragma warning(pop)

namespace editor
{
  Color FromProtocol(const editor::Color4& col);
  Vector3f FromProtocol(const common::Vector3& v);
}