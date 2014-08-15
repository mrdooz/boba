#pragma once
#include "editor_styles.pb.h"
namespace editor
{
namespace protocol
{
void BindStyle(editor::protocol::Style* data, bool *dirty);
void BindStyles(editor::protocol::Styles* data, bool *dirty);
}
}
