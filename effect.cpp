#include "effect.hpp"
#include "graphics.hpp"

using namespace boba;

//------------------------------------------------------------------------------
Effect::Effect(const string& name, u32 id)
  : _name(name)
  , _id(id)
  , _running(false)
  , _mouse_horiz(0)
  , _mouse_vert(0)
  , _mouse_lbutton(false)
  , _mouse_rbutton(false)
  , _mouse_pos_prev(~0)
  , _ctx(GRAPHICS.CreateDeferredContext(true))
  , _first_tick(true)
{
  ZeroMemory(_keystate, sizeof(_keystate));
}

//------------------------------------------------------------------------------
Effect::~Effect()
{
  GRAPHICS.DestroyDeferredContext(_ctx);
}

//------------------------------------------------------------------------------
bool Effect::Show()
{
  return true;
}

//------------------------------------------------------------------------------
bool Effect::Hide()
{
  return true;
}

//------------------------------------------------------------------------------
bool Effect::Init(const protocol::effect::EffectSetting& config)
{
  return true;
}

//------------------------------------------------------------------------------
bool Effect::SaveSettings()
{
  return true;
}

//------------------------------------------------------------------------------
bool Effect::Reset()
{
  return true; 
}

//------------------------------------------------------------------------------
bool Effect::Update(const UpdateState& state)
{
  return true;
}

//------------------------------------------------------------------------------
bool Effect::Render()
{
  return true; 
}

//------------------------------------------------------------------------------
bool Effect::Close()
{
  return true; 
}

//------------------------------------------------------------------------------
void Effect::SetDuration(TimeDuration startTime, TimeDuration endTime)
{
  _startTime = startTime;
  _endTime = endTime;
}

//------------------------------------------------------------------------------
void Effect::SetStartTime(TimeDuration startTime)
{
  _startTime = startTime;
}

//------------------------------------------------------------------------------
void Effect::WndProc(UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message)
  {
    case WM_KEYDOWN:
      // ignore keyup/down if ctrl is pressed
      if (!(GetKeyState(VK_CONTROL) & (1 << 15)))
      {
        if (wParam <= 255)
          _keystate[wParam] = 1;
      }
      break;

    case WM_KEYUP:
      if (!(GetKeyState(VK_CONTROL) & (1 << 15)))
      {
        if (wParam <= 255)
          _keystate[wParam] = 0;
        switch (wParam) {
        case 'F':
          break;
        }
      }
      break;

    case WM_MOUSEMOVE:
      if (_mouse_pos_prev != ~0) {
        _mouse_horiz = LOWORD(lParam) - LOWORD(_mouse_pos_prev);
        _mouse_vert = HIWORD(lParam) - HIWORD(_mouse_pos_prev);
      }
      _mouse_pos_prev = lParam;
      break;

    case WM_LBUTTONDOWN:
      _mouse_lbutton = true;
      break;

    case WM_LBUTTONUP:
      _mouse_lbutton = false;
      break;

    case WM_RBUTTONDOWN:
      _mouse_rbutton = true;
      break;

    case WM_RBUTTONUP:
      _mouse_rbutton = false;
      break;
  }
}


//------------------------------------------------------------------------------
bool Effect::Running() const
{
  return _running; 
}

//------------------------------------------------------------------------------
void Effect::SetRunning(bool b)
{ 
  _running = b; 
}
