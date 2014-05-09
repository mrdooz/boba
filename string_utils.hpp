#pragma once

namespace boba
{
  string to_string(char const * const format, ... );
  string trim(const string &str);
  bool wide_char_to_utf8(LPCOLESTR unicode, size_t len, string *str);
  string wide_char_to_utf8(const std::wstring &str);
  string wide_char_to_utf8(const WCHAR *str);

  wstring utf8_to_wide(const char *str);

  bool begins_with(const char *str, const char *sub_str);
  bool begins_with(const string &str, const string &sub_str);
}
