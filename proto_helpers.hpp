#pragma once
#include "file_utils.hpp"
#include "resource_manager.hpp"

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

}