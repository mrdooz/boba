#pragma once
#include "common.pb.h"
namespace common
{
namespace protocol
{
void BindColor4(common::protocol::Color4* data, bool *dirty);
void BindVector2(common::protocol::Vector2* data, bool *dirty);
void BindVector3(common::protocol::Vector3* data, bool *dirty);
void BindVector4(common::protocol::Vector4* data, bool *dirty);
void BindMatrix3x3(common::protocol::Matrix3x3* data, bool *dirty);
void BindMatrix4x4(common::protocol::Matrix4x4* data, bool *dirty);
void BindVector3Keyframe(common::protocol::Vector3Keyframe* data, bool *dirty);
void BindVector3Anim(common::protocol::Vector3Anim* data, bool *dirty);
}
}
