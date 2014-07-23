#pragma once
#pragma warning(push)
#pragma warning(disable: 4244 4267)
#include "generator.pb.h"
#pragma warning(pop)

namespace generator
{
void BindSpiky(generator::Spiky* data, bool *dirty);
void BindPlane(generator::Plane* data, bool *dirty);
}
