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

}