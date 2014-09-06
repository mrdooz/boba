#include "proto_helpers.hpp"

namespace boba
{
  //------------------------------------------------------------------------------
  void FromProtocol(const ::google::protobuf::RepeatedField<float>& protocol, Vector3* out)
  {
    memcpy(out, protocol.data(), sizeof(Vector3));
  }

  //------------------------------------------------------------------------------
  void FromProtocol(const ::google::protobuf::RepeatedField<float>& protocol, Vector4* out)
  {
    memcpy(out, protocol.data(), sizeof(Vector4));
  }

  //------------------------------------------------------------------------------
  Vector2 FromProtocol(const protocol::common::Vector2& v)
  {
    return Vector2(v.x(), v.y());
  }

  //------------------------------------------------------------------------------
  Vector3 FromProtocol(const protocol::common::Vector3& v)
  {
    return Vector3(v.x(), v.y(), v.z());
  }

  //------------------------------------------------------------------------------
  Vector4 FromProtocol(const protocol::common::Vector4& v)
  {
    return Vector4(v.x(), v.y(), v.z(), v.w());
  }

  //------------------------------------------------------------------------------
  Matrix FromProtocol(const protocol::common::Matrix4x4& m)
  {
    if (!m.has_row_1())
      return Matrix::Identity();

    return Matrix(
      FromProtocol(m.row_1()),
      FromProtocol(m.row_2()),
      FromProtocol(m.row_3()),
      FromProtocol(m.row_4()));
  }

  //------------------------------------------------------------------------------
  void ToProtocol(const Vector2& v, protocol::common::Vector2* out)
  {
    out->set_x(v.x);
    out->set_y(v.y);
  }

  //------------------------------------------------------------------------------
  void ToProtocol(const Vector3& v, protocol::common::Vector3* out)
  {
    out->set_x(v.x);
    out->set_y(v.y);
    out->set_z(v.z);
  }

  //------------------------------------------------------------------------------
  void ToProtocol(const Vector4& v, protocol::common::Vector4* out)
  {
    out->set_x(v.x);
    out->set_y(v.y);
    out->set_z(v.z);
    out->set_w(v.w);
  }

  //------------------------------------------------------------------------------
  void ToProtocol(const Matrix& m, protocol::common::Matrix4x4* out)
  {
    ToProtocol(Vector4(m._11, m._12, m._13, m._14), out->mutable_row_1());
    ToProtocol(Vector4(m._21, m._22, m._23, m._24), out->mutable_row_2());
    ToProtocol(Vector4(m._31, m._32, m._33, m._34), out->mutable_row_3());
    ToProtocol(Vector4(m._41, m._42, m._43, m._44), out->mutable_row_4());
  }


}

