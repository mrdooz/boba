#include "proto_utils.hpp"

namespace editor
{
  Color FromProtocol(const editor::Color4& col)
  {
    return Color(col.r(), col.g(), col.b(), col.a());
  }

}