#include "init_sequence.hpp"

using namespace boba;
using namespace bristol;

//------------------------------------------------------------------------------
InitSequence::InitSequence()
  : _fatal(false)
{
}

//------------------------------------------------------------------------------
void InitSequence::AddFailure(const char* file, int line, const char* str, bool fatal)
{
  _failures.push_back({ file, line, str });
  _fatal |= fatal;
}

//------------------------------------------------------------------------------
bool InitSequence::EndSequence()
{
  if (_failures.empty())
    return true;

  for (const InitFailure& f : _failures)
  {
    LOG_WARN_NAKED(ToString("%s(%d): %s", f.file, f.line, f.str).c_str());
  }

  return false;
}
