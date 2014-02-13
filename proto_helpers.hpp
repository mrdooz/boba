#pragma once
#include "file_utils.hpp"

namespace boba
{
  //------------------------------------------------------------------------------
  template <typename T>
  bool LoadProto(const char* filename, T* msg)
  {
    vector<char> buf;
    if (!LoadFile(filename, &buf))
    {
      return false;
    }

    string str(buf.data(), buf.size());
    return google::protobuf::TextFormat::ParseFromString(str, msg);
  }

}