#pragma once
#include "demo.pb.h"
namespace demo
{
void BindPart(demo::Part* data, bool *dirty);
void BindConfig(demo::Config* data, bool *dirty);
}
