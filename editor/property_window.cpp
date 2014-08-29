#include "property_window.hpp"
#include "editor.hpp"
#include "row_var.hpp"

using namespace editor;
using namespace bristol;

#pragma warning(disable: 4244)

//----------------------------------------------------------------------------------
PropertyWindow::PropertyWindow(
  const string& title,
  const Vector2f& pos,
  const Vector2f& size)
    : VirtualWindow(title, pos, size, bristol::WindowFlags(bristol::WindowFlag::StaticWindow))
    , _var(nullptr)
    , _gui(_texture, _font)
    , _keyboardFocus(false)
{
}

//----------------------------------------------------------------------------------
bool PropertyWindow::Init()
{
  if (!VirtualWindow::Init())
    return false;

  _windowManager->RegisterHandler(Event::MouseButtonPressed, nullptr, bind(&PropertyWindow::OnMouseButtonPressed, this, _1));
  _windowManager->RegisterHandler(Event::MouseMoved, nullptr, bind(&PropertyWindow::OnMouseMoved, this, _1));
  _windowManager->RegisterHandler(Event::MouseButtonReleased, nullptr, bind(&PropertyWindow::OnMouseButtonReleased, this, _1));
  _windowManager->RegisterHandler(Event::KeyReleased, nullptr, bind(&PropertyWindow::OnKeyReleased, this, _1));
  _windowManager->RegisterHandler(Event::TextEntered, nullptr, bind(&PropertyWindow::OnTextEntered, this, _1));

  if (!_font.loadFromFile(EDITOR.GetAppRoot() + "gfx/coders_crux.ttf"))
    return false;

  return true;
}

//----------------------------------------------------------------------------------
void PropertyWindow::Draw()
{
  _texture.clear();

  const editor::protocol::Settings& settings = EDITOR.Settings();

  _texture.draw(CreateRect(_pos, _size, ::FromProtocol(settings.timeline_view_background_color())));

  _gui.BeginFrame();

  if (_var)
  {
    ImGui::WidgetResult  res;
    res = _gui.Button(_gui.WidgetId(), IntRect(10, 10, 50, 40), "test");
    res = _gui.Button(_gui.WidgetId(), IntRect(70, 10, 50, 40), "test");

    res = _gui.EditBox(_gui.WidgetId(), IntRect(10, 60, 100, 20), &_varValue);
    switch (res)
    {
      case ImGui::WidgetResult::LostFocus: _keyboardFocus = false; break;
      case ImGui::WidgetResult::InputDone: _var->_value = atof(_varValue.c_str()); break;
    }
  }

  _gui.EndFrame();
  _texture.display();
}

//----------------------------------------------------------------------------------
void PropertyWindow::Update()
{
  _gui._uiState.keyMod =
      Keyboard::isKeyPressed(Keyboard::Key::LShift) ? UiState::KEYBOARD_SHIFT : 0 |
      Keyboard::isKeyPressed(Keyboard::Key::LControl) ? UiState::KEYBOARD_CONTROL : 0 |
      Keyboard::isKeyPressed(Keyboard::Key::LAlt) ? UiState::KEYBOARD_ALT : 0;
}

//----------------------------------------------------------------------------------
void PropertyWindow::SendEffectEvent(RowVar* sender, const EffectRowEvent& event)
{
  if (event.type == EffectRowEvent::Type::VarSelected)
  {
    _var = sender;
    _varValue = to_string("%.3f", _var->_value);
  }
}

//----------------------------------------------------------------------------------
bool PropertyWindow::OnMouseButtonPressed(const Event& event)
{
  _gui._uiState.mouseButton |= (1 << event.mouseButton.button);
  _gui._uiState.mousePos = Vector2i(event.mouseButton.x, event.mouseButton.y);
  return false;
}

//----------------------------------------------------------------------------------
bool PropertyWindow::OnMouseMoved(const Event& event)
{
  _gui._uiState.mousePos = Vector2i(event.mouseMove.x, event.mouseMove.y);
  return false;
}

//----------------------------------------------------------------------------------
bool PropertyWindow::OnMouseButtonReleased(const Event& event)
{
  _gui._uiState.mouseButton &= ~(1 << event.mouseButton.button);
  _gui._uiState.mousePos = Vector2i(event.mouseButton.x, event.mouseButton.y);

  return false;
}

//----------------------------------------------------------------------------------
bool PropertyWindow::OnKeyReleased(const Event& event)
{
  _gui._uiState.key = event.key.code;
  return _keyboardFocus;
}

//----------------------------------------------------------------------------------
bool PropertyWindow::OnTextEntered(const Event& event)
{
  u32 k = event.text.unicode;
  if ((k & 0xff80) == 0)
    _gui._uiState.keyChar = k & 0x7f;

  return _keyboardFocus;
}
