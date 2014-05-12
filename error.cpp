#include "error.hpp"
#include "app.hpp"

//-----------------------------------------------------------------------------
namespace boba
{
  vector<LogSink*> g_logSinks;
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
LogSink::LogSink()
{
  g_logSinks.push_back(this);
}

//-----------------------------------------------------------------------------
LogSink::~LogSink()
{
  auto it = find(g_logSinks.begin(), g_logSinks.end(), this);
  if (it != g_logSinks.end())
    g_logSinks.erase(it);
}

//-----------------------------------------------------------------------------
LogSinkFile::LogSinkFile()
    : _file(nullptr)
{
}

//-----------------------------------------------------------------------------
LogSinkFile::~LogSinkFile()
{
  if (_file)
    fclose(_file);
}

//-----------------------------------------------------------------------------
bool LogSinkFile::Open(const char* filename)
{
  _file = fopen(filename, "at");
  return !!_file;
}

//-----------------------------------------------------------------------------
void LogSinkFile::Log(LogLevel level, const LogEntry& entry)
{
  // create log prefix, with severity and time stamp
  static char levelPrefix[] = { '-', 'D', 'I', 'W', 'E' };

  ptime now = microsec_clock::local_time();
  string str = ToString("[%c] %s - ", levelPrefix[(int)level],
    boost::posix_time::to_iso_extended_string(now).c_str());

  if (_file)
    fprintf(_file, "%s", str.c_str());

  const vector<pair<string, string>>& msg = entry.values;
  for (size_t i = 0; i < msg.size(); ++i)
  {
    auto& p = msg[i];
    bool last = i == msg.size() - 1;
    str = ToString("%s=%s%c", p.first.c_str(), p.second.c_str(), last ? '\n' : '|');
    if (_file)
      fprintf(_file, "%s", str.c_str());
  }

  if (_file)
    fflush(_file);
}

//-----------------------------------------------------------------------------
void LogSinkConsole::Log(LogLevel level, const LogEntry& entry)
{
  // create clickable console prefix
  string str = ToString("%s(%d): %s", entry.file, entry.line, entry.desc.empty() ? "" : entry.desc.c_str());

  const vector<pair<string, string>>& msg = entry.values;
  for (size_t i = 0; i < msg.size(); ++i)
  {
    auto& p = msg[i];
    bool last = i == msg.size() - 1;
    str = ToString("%s=%s%c", p.first.c_str(), p.second.c_str(), last ? '\n' : '|');
  }

  str += '\n';
  OutputDebugStringA(str.c_str());
}

//-----------------------------------------------------------------------------
void LogSinkApp::Log(LogLevel level, const LogEntry& entry)
{
  // only log entrys with a desc
  if (entry.desc.empty() || level == LogLevel::Debug)
    return;

  MessageType type = 
    level == LogLevel::Info ? MessageType::Info :
    level == LogLevel::Warning ? MessageType::Warning :
    MessageType::Error;

  APP.AddMessage(type, entry.desc);
}

//-----------------------------------------------------------------------------
LogStream::LogStream(LogLevel level, const char* file, u32 line)
    : _level(level)
{
  _entry.file = file;
  _entry.line = line;
}

//-----------------------------------------------------------------------------
LogStream::~LogStream()
{
  if (_level < g_logLevel)
    return;

  for (LogSink* sink : g_logSinks)
    sink->Log(_level, _entry);
}

//-----------------------------------------------------------------------------
void LogStream::Append(const string& key, const string& value)
{
  if (_level < g_logLevel)
    return;

  _entry.values.push_back(make_pair(key, value));
}

//-----------------------------------------------------------------------------
LogStream& boba::operator<<(LogStream& s, const char* desc)
{
  if (s._entry.desc.empty())
  {
    s._entry.desc = desc;
  }
  else
  {
    s.Append("", desc);
  }
  return s;
}

//-----------------------------------------------------------------------------
LogLevel GetLogLevel();
