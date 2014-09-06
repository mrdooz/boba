#pragma once

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable: 4244 4267)
#endif

#include "common_proto.hpp"
#include "editor_proto.hpp"
#include "common_proto.hpp"
#include "editor_settings.pb.h"
#include "proto_helpers.hpp"

#ifdef _WIN32
#pragma warning(pop)
#endif

namespace boba
{
  struct Settings;
  struct StyleSetting;
  struct StyleSettings;

  Settings FromProtocol(const protocol::editor::Settings& p);
  void ToProtocol(const Settings& v, protocol::editor::Settings* p);
  StyleSetting FromProtocol(const protocol::editor::StyleSetting& p);
  void ToProtocol(const StyleSetting& v, protocol::editor::StyleSetting* p);
  StyleSettings FromProtocol(const protocol::editor::StyleSettings& p);
  void ToProtocol(const StyleSettings& v, protocol::editor::StyleSettings* p);
}
