#include "property_window.hpp"
#include "editor.hpp"
#include "row_var.hpp"
#include "proto_helpers.hpp"

using namespace editor;
using namespace bristol;

using namespace editor::common;

#pragma warning(disable: 4244)

//----------------------------------------------------------------------------------
PropertyWindow::PropertyWindow(
  const string& title,
  const Vector2f& pos,
  const Vector2f& size)
    : VirtualWindow(title, pos, size, bristol::WindowFlags(bristol::WindowFlag::StaticWindow))
    , _var(nullptr)
    , _anim(nullptr)
    , _keyframe(nullptr)
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

  const protocol::editor::Settings& settings = EDITOR.Settings();

  _texture.draw(CreateRect(_pos, _size, ::FromProtocol(settings.timeline_view_background_color())));

  static bool open = true;
  ImGui::Begin("Properties", &open, ImVec2(200, 100));

  if (_anim)
  {
    static const char* interpolationTypes[] = { "Linear", "Bezier", "Catmull Rom" };
    ImGui::Combo("Interpolation", (int*)&_anim->header.type, interpolationTypes, 3);

    static const char* loopTypes[] = { "Clamp", "Loop", "Loop Smooth" };
    ImGui::Combo("Loop", (int*)&_anim->header.loop, loopTypes, 3);

    bool useMinValue = _anim->valueFlags.IsSet(FloatAnim::ValueFlagsF::UseMinValue);
    bool useMaxValue = _anim->valueFlags.IsSet(FloatAnim::ValueFlagsF::UseMaxValue);
    if (ImGui::Checkbox("Use Min Value", &useMinValue)) _anim->valueFlags.Toggle(FloatAnim::ValueFlagsF::UseMinValue);
    if (useMinValue) ImGui::InputFloat("Min Value", &_anim->minValue);

    if (ImGui::Checkbox("Use Max Value", &useMaxValue)) _anim->valueFlags.Toggle(FloatAnim::ValueFlagsF::UseMaxValue);
    if (useMaxValue) ImGui::InputFloat("Max Value", &_anim->maxValue);
  }
  else if (_keyframe)
  {
    int t = (int)_keyframe->key.time;
    ImGui::InputInt("time", &t, 100, 2500);
    _keyframe->key.time = max(0, t);
    ImGui::InputFloat("value", &_keyframe->key.value);
  }
  ImGui::End();

  _texture.display();
}

//----------------------------------------------------------------------------------
void PropertyWindow::Update()
{
}

//----------------------------------------------------------------------------------
void PropertyWindow::SendEffectEvent(RowVar* sender, const EffectRowEvent& event)
{
  switch (event.type)
  {
    case EffectRowEvent::Type::VarSelected:
      _var = sender;
      _anim = (FloatAnim*)event.data;
      _keyframe = nullptr;
      break;

    case EffectRowEvent::Type::KeyframeUpdated:
    case EffectRowEvent::Type::KeyframeSelected:
      _var = sender;
      _keyframe = (FloatKeyframe*)event.data;
      _anim = nullptr;
      break;
  }
}

//----------------------------------------------------------------------------------
bool PropertyWindow::OnMouseButtonPressed(const Event& event)
{
  return false;
}

//----------------------------------------------------------------------------------
bool PropertyWindow::OnMouseMoved(const Event& event)
{
  return false;
}

//----------------------------------------------------------------------------------
bool PropertyWindow::OnMouseButtonReleased(const Event& event)
{
  return false;
}

//----------------------------------------------------------------------------------
bool PropertyWindow::OnKeyReleased(const Event& event)
{
  return _keyboardFocus;
}

//----------------------------------------------------------------------------------
bool PropertyWindow::OnTextEntered(const Event& event)
{
  return _keyboardFocus;
}
