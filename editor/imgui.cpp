#include "imgui.hpp"
#include "effect_utils.hpp"


using namespace editor;

//----------------------------------------------------------------------------------
ImGui::ImGui(sf::RenderTexture& texture, sf::Font& font)
    : _texture(texture)
    , _font(font)
    , _nextWidgetId(1)
{
  ResetState();
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
  Text text = CreateText(_font, {p.x, p.y-s.y/4}, rect.height, str->c_str());
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
  bool isHot          = _uiState.hotItem == id;
  bool isActive       = _uiState.activeItem == id;
  bool keyboardFocus  = _uiState.keyboardItem == id;

  Vector2f ofs = 0.05f * SizeFromRect<int, float>(rect);

  // show if we have keyboard focus
  if (keyboardFocus)
    _texture.draw(CreateRect(PosFromRect<int, float>(rect) - ofs, SizeFromRect<int, float>(rect) + 3.0f * ofs, Color(200, 0, 0,255)));

  _texture.draw(CreateRect(PosFromRect<int, float>(rect) + ofs, SizeFromRect<int, float>(rect), Color(0,0,0,255)));

  // active << 1 | hot
  Color cols[4] = { Color(150, 150, 150, 255), Color(200, 200, 200, 255), Color(255, 255, 255, 255), Color(255, 255, 255, 255) };
  Color col = cols[(isActive << 1) | isHot];
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
