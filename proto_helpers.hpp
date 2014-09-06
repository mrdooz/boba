#pragma once
#include "resource_manager.hpp"

#pragma warning(push)
#pragma warning(disable: 4244 4267)
#include "protocol/common.pb.h"
#pragma warning(pop)

namespace boba
{
  //------------------------------------------------------------------------------
  template <typename T>
  bool LoadProto(const char* filename, T* msg)
  {
    vector<char> buf;
    if (!RESOURCE_MANAGER.LoadFile(filename, &buf))
    {
      return false;
    }

    string str(buf.data(), buf.size());
    return google::protobuf::TextFormat::ParseFromString(str, msg);
  }

  //------------------------------------------------------------------------------
  void FromProtocol(const ::google::protobuf::RepeatedField<float>& protocol, Vector3* out);
  void FromProtocol(const ::google::protobuf::RepeatedField<float>& protocol, Vector4* out);

  Vector2 FromProtocol(const protocol::common::Vector2& v);
  Vector3 FromProtocol(const protocol::common::Vector3& v);
  Vector4 FromProtocol(const protocol::common::Vector4& v);
  //Matrix FromProtocol(const protocol::common::Matrix3x3& v);
  Matrix FromProtocol(const protocol::common::Matrix4x4& m);

  void ToProtocol(const Vector2& v, protocol::common::Vector2* out);
  void ToProtocol(const Vector3& v, protocol::common::Vector3* out);
  void ToProtocol(const Vector4& v, protocol::common::Vector4* out);
  void ToProtocol(const Matrix& m, protocol::common::Matrix4x4* out);
}