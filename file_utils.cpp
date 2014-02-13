#include "file_utils.hpp"
#include "utils.hpp"

namespace boba
{
  //------------------------------------------------------------------------------
  bool LoadFile(const char* filename, vector<char> *buf)
  {
    ScopedHandle h(CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL));
    if (!h)
      return false;

    DWORD size = GetFileSize(h, NULL);
    if (size > 0)
    {
      buf->resize(size);
      DWORD res;
      if (!ReadFile(h, &(*buf)[0], size, &res, NULL)) 
        return false;
    }
    return true;
  }

  //------------------------------------------------------------------------------
  bool FileExists(const char* filename)
  {
    struct _stat status;
    return _access(filename, 0) == 0 && _stat(filename, &status) == 0 && (status.st_mode & _S_IFREG);
  }

  //------------------------------------------------------------------------------
  bool DirectoryExists(const char* name)
  {
    struct _stat status;
    return _access(name, 0) == 0 && _stat(name, &status) == 0 && (status.st_mode & _S_IFDIR);
  }

  //------------------------------------------------------------------------------
  bool SaveFile(const char *filename, const void *buf, int len)
  {
    FILE *f = fopen(filename, "wb");
    if (!f)
      return false;

    DEFER([=]() {
      fclose(f);
    });

    return fwrite(buf, len, 1, f) == len;
  }
}
