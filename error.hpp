#pragma once
#include "utils.hpp"

namespace boba
{
  enum class LogLevel
  {
    None,
    Debug,
    Info,
    Warning,
    Error,
  };

  struct LogEntry
  {
    const char* file;
    u32 line;
    string desc;
    vector<pair<string, string>> values;
  };

  //----------------------------------------------------------------------------------
  template <typename T>
  struct LogObject
  {
    LogObject(const string& key, const T& value) : key(key), value(value) {}
    string key;
    T value;
  };

  //----------------------------------------------------------------------------------
  template <typename T>
  LogObject<T> LogKeyValue(const string& key, const T& value)
  {
    return LogObject<T>(key, value);
  }

  //----------------------------------------------------------------------------------
  struct LogSink
  {
    LogSink();
    virtual ~LogSink();
    virtual void Log(LogLevel level, const LogEntry& entry) = 0;
  };

  //----------------------------------------------------------------------------------
  struct LogSinkFile : public LogSink
  {
    LogSinkFile();
    ~LogSinkFile();

    bool Open(const char* filename);
    virtual void Log(LogLevel level, const LogEntry& entry);

    FILE* _file;
  };

  //----------------------------------------------------------------------------------
  struct LogSinkConsole : public LogSink
  {
    virtual void Log(LogLevel level, const LogEntry& entry);
  };

  //----------------------------------------------------------------------------------
  struct LogSinkApp : public LogSink
  {
    virtual void Log(LogLevel level, const LogEntry& entry);
  };

  //----------------------------------------------------------------------------------
  struct LogStream
  {
    LogStream(LogLevel level, const char* file, u32 line);
    ~LogStream();

    void Append(const string& key, const string& value);

    LogEntry _entry;
    LogLevel _level;
  };

  template <typename T>
  LogStream& operator<<(LogStream& s, const LogObject<T>& lhs)
  {
    std::ostringstream str;
    str << lhs.value;
    s.Append(lhs.key, str.str());
    return s;
  }

  LogStream& operator<<(LogStream& s, const char* desc);

  // The minimum level at which we log
  LogLevel GetLogLevel();
  void SetLogLevel(LogLevel level);

#define LOG_DEBUG(x)  \
    if (GetLogLevel() <= boba::LogLevel::Debug) {  \
      boba::LogStream GEN_NAME(s, __LINE__)(boba::LogLevel::Debug, __FILE__, __LINE__); GEN_NAME(s, __LINE__) << x  \
    }

#define LOG_INFO(x) \
  boba::LogStream GEN_NAME(s, __LINE__)(boba::LogLevel::Info, __FILE__, __LINE__); GEN_NAME(s, __LINE__) << x
#define LOG_WARN(x) \
  boba::LogStream GEN_NAME(s, __LINE__)(boba::LogLevel::Warning, __FILE__, __LINE__); GEN_NAME(s, __LINE__) << x
#define LOG_ERROR(x)  \
  boba::LogStream GEN_NAME(s, __LINE__)(boba::LogLevel::Error, __FILE__, __LINE__); GEN_NAME(s, __LINE__) << x
}

