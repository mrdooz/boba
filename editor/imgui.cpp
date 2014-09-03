#include "imgui.hpp"
#include "effect_utils.hpp"


using namespace editor;

//#####  Palette URL: http://paletton.com/#uid=50X0M0kl2lD0pZNaDw6vwbaZt0y
//*** Primary color: rgb(172,135, 59) rgb(255,254,252) rgb(255,227,170) rgb( 89, 60,  1) rgb(  4,  3,  0)4
//*** Secondary color (1): rgb(172,165, 59) rgb(255,255,252) rgb(255,249,170) rgb( 89, 83,  1) rgb(  4,  4,  0)
//*** Secondary color (2): rgb(172, 85, 59) rgb(255,253,252) rgb(255,189,170) rgb( 89, 21,  1) rgb(  4,  1,  0)

namespace
{
  // active << 1 | hot
  Color g_cols[4] = { Color(89, 60,  1), Color(255,227,170), Color(172,135, 59), Color(255,254,252) };
}

//----------------------------------------------------------------------------------
ImGui::ImGui(sf::RenderTexture& texture, sf::Font& font)
    : _texture(texture)
    , _font(font)
    , _nextWidgetId(1)
{
  ResetState();
}

//----------------------------------------------------------------------------------
void ImGui::Label(u32 id, const IntRect& rect, const char* label)
{
  DrawWidget(id, rect);

  Vector2f s = SizeFromRect<int, float>(rect);
  Vector2f p = PosFromRect<int, float>(rect);

  Text text = CreateText(_font, {p.x, p.y-s.y/4}, 16, label);
  _texture.draw(text);
}

//----------------------------------------------------------------------------------
ImGui::WidgetResult ImGui::Button(u32 id, const IntRect& rect, const char* label)
{
  bool isHot, isActive, hasKeyboardFocus;
  FocusCheck(id, rect, &isHot, &isActive, &hasKeyboardFocus);
  DrawWidget(id, rect);

  if (hasKeyboardFocus)
  {
    switch (_uiState.key)
    {
      case Keyboard::Key::Tab:
        _uiState.keyboardItem = 0;
        _uiState.key = Keyboard::Key::Unknown;

        if (_uiState.keyMod & UiState::KEYBOARD_SHIFT)
          _uiState.keyboardItem = _uiState.lastWidget;
        break;

      case Keyboard::Key::Return:
        return WidgetResult::ButtonPressed;
    }
  }

  _uiState.lastWidget = id;

  // if button is hot & active, but the mouse isn't down, button was clicked
  return (_uiState.mouseButton == 0 && isHot && isActive) ? WidgetResult::ButtonPressed : WidgetResult::None;
}

//----------------------------------------------------------------------------------
ImGui::WidgetResult ImGui::EditBox(u32 id, const IntRect& rect, string* str)
{
  bool isHot, isActive, hasKeyboardFocus;
  FocusCheck(id, rect, &isHot, &isActive, &hasKeyboardFocus);

  DrawWidget(id, rect);

  Vector2f s = SizeFromRect<int, float>(rect);
  Vector2f p = PosFromRect<int, float>(rect);

  string tmpString = *str;
  if (hasKeyboardFocus)
  {
    // add blinking caret
    time_duration now = microsec_clock::local_time().time_of_day();
    if (now.total_seconds() & 1)
      tmpString += '_';
  }

  Text text = CreateText(_font, {p.x, p.y-s.y/4}, 16, tmpString.c_str());

  _texture.draw(text);

  if (hasKeyboardFocus)
  {
    switch (_uiState.key)
    {
      case Keyboard::Key::Tab:
        _uiState.keyboardItem = 0;
        _uiState.key = Keyboard::Key::Unknown;

        if (_uiState.keyMod & UiState::KEYBOARD_SHIFT)
          _uiState.keyboardItem = _uiState.lastWidget;
        break;

      case Keyboard::Key::BackSpace:
        if (!str->empty())
          str->pop_back();
        break;

      case Keyboard::Key::Return:
        return WidgetResult::InputDone;

      case Keyboard::Key::Escape:
        return WidgetResult::InputCancelled;
    }

    if (_uiState.keyChar >= 32 && _uiState.keyChar < 127)
      str->push_back(_uiState.keyChar);
  }

  _uiState.lastWidget = id;

  // if button is hot & active, but the mouse isn't down, take keyboard focus
  if (_uiState.mouseButton == 0 && isHot && isActive)
    _uiState.keyboardItem = id;

  // return value indicates if the widget has keyboard focus
  return _uiState.keyboardItem != id ? WidgetResult::LostFocus : WidgetResult::None;
}

//----------------------------------------------------------------------------------
ImGui::WidgetResult ImGui::CheckBox(u32 id, const IntRect& rect, const char* label, bool* value)
{
  Vector2f tmpS = SizeFromRect<int, float>(rect);
  Vector2f tmpP = PosFromRect<int, float>(rect);

  Text text = CreateText(_font, {tmpP.x, tmpP.y-tmpS.y/4}, 16, label);
  FloatRect r = text.getLocalBounds();

  IntRect checkboxRect(rect);
  checkboxRect.left += (int)(r.width + 5);

  bool isHot, isActive, hasKeyboardFocus;
  FocusCheck(id, checkboxRect, &isHot, &isActive, &hasKeyboardFocus);

  Vector2f s = SizeFromRect<int, float>(checkboxRect);
  Vector2f p = PosFromRect<int, float>(checkboxRect);

  _texture.draw(text);

  Vector2f ofs = 0.05f * SizeFromRect<int, float>(checkboxRect);

  // show if we have keyboard focus
  if (hasKeyboardFocus)
    _texture.draw(CreateRect(p - ofs, s + 2.0f * ofs, Color(200, 0, 0,255)));

  // active << 1 | hot
  Color col = g_cols[((int)isActive << 1) | (int)isHot];
  float f = 0.5f;
  col = *value ? col : Color(f*col.r, f*col.g, f*col.b);
  _texture.draw(CreateRect(p, s, col));

  // if button is hot & active, but the mouse isn't down, button was clicked
  return (_uiState.mouseButton == 0 && isHot && isActive) ? WidgetResult::ButtonPressed : WidgetResult::None;
}


//----------------------------------------------------------------------------------
void ImGui::FocusCheck(u32 id, const IntRect& rect, bool* isHot, bool* isActive, bool* hasKeyboardFocus)
{
  // check if the widget is hot
  if (rect.contains(_uiState.mousePos))
  {
    _uiState.hotItem = id;

    // if left mouse button is down, mark widget as active
    if (_uiState.mouseButton & UiState::MOUSE_LEFT)
      _uiState.activeItem = id;
  }

  // if no widget has keyboard focus, take it
  if (_uiState.keyboardItem == 0)
    _uiState.keyboardItem = id;

  *isHot = _uiState.hotItem == id;
  *isActive = _uiState.activeItem == id;
  *hasKeyboardFocus = _uiState.keyboardItem == id;
}

//----------------------------------------------------------------------------------
void ImGui::DrawWidget(u32 id, const IntRect& rect)
{
  bool isHot            = _uiState.hotItem == id;
  bool isActive         = _uiState.activeItem == id;
  bool hasKeyboardFocus = _uiState.keyboardItem == id;

  Vector2f ofs = 0.05f * SizeFromRect<int, float>(rect);

  // show if we have keyboard focus
  if (hasKeyboardFocus)
    _texture.draw(CreateRect(PosFromRect<int, float>(rect) - ofs, SizeFromRect<int, float>(rect) + 2.0f * ofs, Color(200, 0, 0,255)));

  // active << 1 | hot
  Color col = g_cols[((int)isActive << 1) | (int)isHot];
  _texture.draw(CreateRect(PosFromRect<int, float>(rect), SizeFromRect<int, float>(rect), col));
}

//----------------------------------------------------------------------------------
void ImGui::BeginFrame()
{
  _uiState.hotItem = 0;
  _nextWidgetId = 1;
}

//----------------------------------------------------------------------------------
void ImGui::EndFrame()
{
  if (_uiState.mouseButton == 0)
  {
    _uiState.activeItem = 0;
  }
  else
  {
    if (_uiState.activeItem == 0)
    {
      _uiState.activeItem = -1;
    }
  }

  _uiState.key = Keyboard::Key::Unknown;
  _uiState.keyChar = 0;
}

//----------------------------------------------------------------------------------
u32 ImGui::WidgetId()
{
  return _nextWidgetId++;
}

//----------------------------------------------------------------------------------
void ImGui::ResetState()
{
  memset(&_uiState, 0, sizeof(_uiState));
  _uiState.key = Keyboard::Key::Unknown;
  _nextWidgetId = 1;
}
