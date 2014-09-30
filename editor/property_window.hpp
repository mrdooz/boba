#pragma once

namespace editor
{
  struct RowVar;
  struct EffectRowEvent;

  namespace common
  {
    struct FloatAnim;
    struct FloatKeyframe;
  }

  //----------------------------------------------------------------------------------
  class PropertyWindow : public VirtualWindow
  {
  public:
    PropertyWindow(
        const string& title,
        const Vector2f& pos,
        const Vector2f& size);

    virtual bool Init();
    virtual void Draw();
    virtual void Update();

    void SendEffectEvent(RowVar* sender, const EffectRowEvent& event);

    bool OnMouseButtonPressed(const Event& event);
    bool OnMouseMoved(const Event& event);
    bool OnMouseButtonReleased(const Event& event);
    bool OnKeyReleased(const Event& event);
    bool OnTextEntered(const Event& event);

    RowVar* _var;
    common::FloatAnim* _anim;
    common::FloatKeyframe* _keyframe;

    bool _keyboardFocus = false;
  };

}