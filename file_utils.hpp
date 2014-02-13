#pragma once

namespace boba
{
  bool LoadFile(const char* filename, vector<char>* buf);
  bool SaveFile(const char* filename, const void* buf, int len);
  bool FileExists(const char* filename);
  bool DirectoryExists(const char *name);
}
