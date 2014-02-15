#include "error.hpp"

//-----------------------------------------------------------------------------
namespace boba
{
  LogSinkFile g_logSinkFile;
  LogLevel g_logLevel = LogLevel::None;

  //-----------------------------------------------------------------------------
  LogLevel GetLogLevel()
  {
    return g_logLevel;
  }

  //-----------------------------------------------------------------------------
  void SetLogLevel(LogLevel level)
  {
    g_logLevel = level;
  }
}

using namespace boba;

//-----------------------------------------------------------------------------
void LogSink::Log(LogLevel level, const vector<pair<string, string> >& msg)
{
}

//-----------------------------------------------------------------------------
LogSinkFile::LogSinkFile()
    : _log(nullptr)
{
}

//-----------------------------------------------------------------------------
LogSinkFile::~LogSinkFile()
{
  if (_log)
    fclose(_log);
}

//-----------------------------------------------------------------------------
bool LogSinkFile::Open(const char* filename)
{
  _log = fopen(filename, "at");
  return !!_log;
}

//-----------------------------------------------------------------------------
void LogSinkFile::Log(LogLevel level, const vector<pair<string, string> >& msg)
{
  static char levelPrefix[] = { '-', 'D', 'I', 'W', 'E' };

  ptime now = microsec_clock::local_time();
  string str = ToString("[%c] %s - ", levelPrefix[(int)level],
    boost::posix_time::to_iso_extended_string(now).c_str());

  if (_log)
    fprintf(_log, "%s", str.c_str());
  DebugOutput("%s", str.c_str());

  for (size_t i = 0; i < msg.size(); ++i)
  {
    auto& p = msg[i];
    bool last = i == msg.size() - 1;
    str = ToString("%s=%s%c", p.first.c_str(), p.second.c_str(), last ? '\n' : '|');
    if (_log)
      fprintf(_log, "%s", str.c_str());
    DebugOutput("%s", str.c_str());
  }
  if (_log)
    fflush(_log);
}

//-----------------------------------------------------------------------------
LogStream::LogStream(LogSink* sink, LogLevel level)
    : _sink(sink)
    , _level(level)
{
}

//-----------------------------------------------------------------------------
LogStream::~LogStream()
{
  if (_level < g_logLevel)
    return;

  if (_sink)
    _sink->Log(_level, _output);
}

//-----------------------------------------------------------------------------
void LogStream::Append(const string& key, const string& value)
{
  if (_level < g_logLevel)
    return;

  _output.push_back(make_pair(key, value));
}

//-----------------------------------------------------------------------------
LogStream& boba::operator<<(LogStream& s, const char* desc)
{
  std::ostringstream str;
  str << desc;
  s.Append("desc", desc);
  return s;
}

LogLevel GetLogLevel();
