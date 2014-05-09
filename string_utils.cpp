#include "string_utils.hpp"

namespace boba
{
  string to_string(char const * const format, ... ) 
  {
    va_list arg;
    va_start(arg, format);

    const int len = _vscprintf(format, arg) + 1;

    char* buf = (char*)_alloca(len);
    vsprintf_s(buf, len, format, arg);
    va_end(arg);

    return string(buf);
  }

  string wide_char_to_utf8(const WCHAR *str) {
    int len = wcslen(str);
    char *buf = (char *)_alloca(len*2 + 1);
    int res;
    if (!(res = WideCharToMultiByte(CP_UTF8, 0, str, len, buf, len * 2 + 1, NULL, NULL)))
      return false;

    buf[len] = '\0';
    return string(buf);
  }

  bool wide_char_to_utf8(LPCOLESTR unicode, size_t len, string *str)
  {
    if (!unicode)
      return false;

    char *buf = (char *)_alloca(len*2 + 1);

    int res;
    if (!(res = WideCharToMultiByte(CP_UTF8, 0, unicode, len, buf, len * 2 + 1, NULL, NULL)))
      return false;

    buf[len] = '\0';

    *str = string(buf);
    return true;
  }

  string trim(const string &str) 
  {
    int leading = 0, trailing = 0;
    while (isspace((uint8_t)str[leading]))
      leading++;
    const int end = str.size() - 1;
    while (isspace((uint8_t)str[end - trailing]))
      trailing++;

    return leading || trailing ? str.substr(leading, str.size() - (leading + trailing)) : str;
  }

  bool begins_with(const char *str, const char *sub_str) {
    const size_t len_a = strlen(str);
    const size_t len_b = strlen(sub_str);
    if (len_a < len_b)
      return false;

    for (;*sub_str; ++str, ++sub_str) {
      if (*sub_str != *str)
        return false;
    }

    return true;
  }

  bool begins_with(const string &str, const string &sub_str) {
    const size_t len_a = str.size();
    const size_t len_b = sub_str.size();
    if (len_a < len_b)
      return false;

    for (size_t i = 0; i < len_b; ++i) {
      if (sub_str[i] != str[i])
        return false;
    }

    return true;
  }

  wstring utf8_to_wide(const char *str)
  {
    const int len = strlen(str);
    WCHAR *buf = (WCHAR *)_alloca((len + 1) * 2);

    wstring res;
    if (MultiByteToWideChar(CP_UTF8, 0, str, -1, buf, (len + 1) * 2)) {
      res = wstring(buf);
    }
    else {
      int err = GetLastError();
    }
    return res;
  }

}

