#pragma once
#include "common.pb.h"
namespace common
{
void BindVector2(common::Vector2* data, bool *dirty);
void BindVector3(common::Vector3* data, bool *dirty);
void BindVector4(common::Vector4* data, bool *dirty);
void BindMatrix3x3(common::Matrix3x3* data, bool *dirty);
void BindMatrix4x4(common::Matrix4x4* data, bool *dirty);
}
