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

  Vector2 FromProtocol(const common::protocol::Vector2& v);
  Vector3 FromProtocol(const common::protocol::Vector3& v);
  Vector4 FromProtocol(const common::protocol::Vector4& v);
  //Matrix FromProtocol(const common::protocol::Matrix3x3& v);
  Matrix FromProtocol(const common::protocol::Matrix4x4& m);

  void ToProtocol(const Vector2& v, common::protocol::Vector2* out);
  void ToProtocol(const Vector3& v, common::protocol::Vector3* out);
  void ToProtocol(const Vector4& v, common::protocol::Vector4* out);
  void ToProtocol(const Matrix& m, common::protocol::Matrix4x4* out);
}