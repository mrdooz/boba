#pragma once

#if 0

namespace editor
{
  struct UiState
  {
    enum
    {
      KEYBOARD_SHIFT    = 1 << 0,
      KEYBOARD_ALT      = 1 << 1,
      KEYBOARD_CONTROL  = 1 << 2,
    };

    // maps to sf::Mouse::Button
    enum
    {
      MOUSE_LEFT        = 1 << 0,
      MOUSE_RIGHT       = 1 << 1,
      MOUSE_MIDDLE      = 1 << 2,
      MOUSE_XBUTTON_1   = 1 << 3,
      MOUSE_XBUTTON_2   = 1 << 4,
    };

    Vector2i mousePos;
    u32 mouseButton;

    int hotItem;
    int activeItem;

    int keyboardItem;
    Keyboard::Key key;
    char keyChar;
    u32 keyMod;

    int lastWidget;
  };

  struct ImGui
  {
    enum class WidgetResult
    {
      None,
      ButtonPressed,
      LostFocus,
      InputDone,
      InputCancelled,
    };

    ImGui(sf::RenderTexture& texture, sf::Font& font);
    void ResetState();
    void BeginFrame();
    void EndFrame();

    u32 WidgetId();

    void Label(u32 id, const IntRect& rect, const char* label);
    WidgetResult Button(u32 id, const IntRect& rect, const char* label);
    WidgetResult EditBox(u32 id, const IntRect& rect, string* str);
    WidgetResult CheckBox(u32 id, const IntRect& rect, const char* label, bool* value);

    // check if hot/active state should be set to current widget
    void FocusCheck(u32 id, const IntRect& rect, bool* isHot, bool* isActive, bool* hasKeyboardFocus);
    void DrawWidget(u32 id, const IntRect& rect);

    sf::RenderTexture& _texture;
    sf::Font& _font;

    UiState _uiState;
    u32 _nextWidgetId;
  };

}
#endif