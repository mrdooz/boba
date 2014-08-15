#pragma once
#include "editor_settings.pb.h"
namespace editor
{
namespace protocol
{
void BindSettings(editor::protocol::Settings* data, bool *dirty);
}
}
