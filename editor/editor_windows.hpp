#include "style_factory.hpp"

#pragma once

namespace editor
{

  struct Plexus;
  struct StyleFactory;

  //----------------------------------------------------------------------------------
  class PropertyWindow : public VirtualWindow
  {
  public:
    PropertyWindow(
        const string& title,
        const Vector2f& pos,
        const Vector2f& size);
  };

  //----------------------------------------------------------------------------------
  class PreviewWindow : public VirtualWindow
  {
  public:
    PreviewWindow(
        const string& title,
        const Vector2f& pos,
        const Vector2f& size);
  };

  //----------------------------------------------------------------------------------
  class TimelineWindow : public VirtualWindow
  {
  public:
    TimelineWindow(
        const string& title,
        const Vector2f& pos,
        const Vector2f& size);
    ~TimelineWindow();

    virtual bool Init();
    virtual void Draw();

    int TimeToPixel(const time_duration& t);
    time_duration PixelToTime(int x);
    time_duration AbsPixelToTime(int x);

    static TimelineWindow* _instance;

  private:

    void DrawEffects();
    void DrawTimeline();

    struct EffectRow
    {
      struct RowFlagsF
      {
        enum Enum { Expanded = 1 << 0, Selected = 1 << 1 };
        struct Bits { u32 expanded : 1; u32 selected; };
      };

      typedef Flags<RowFlagsF> RowFlags;

      EffectRow(
          const Font& font,
          const string& str,
          EffectRow* parent = nullptr);

      void Draw(RenderTexture& texture);
      static void Flatten(EffectRow* cur, vector<TimelineWindow::EffectRow*>* res);
      static void Reposition(EffectRow* cur, float curY, float rowHeight);
      static float RowHeight(EffectRow* cur, float rowHeight);
      virtual void DrawVars(RenderTexture& texture) {}
      virtual float NumVars() { return 0; }
      virtual void InitVarEdit(float x, float y) {}
      virtual void EndEditVars(bool cancel) {}
      virtual void UpdateEditVar(Keyboard::Key key) {}
      virtual bool KeyframeIntersect(const Vector2f& pt) { return false; }
      virtual void UpdateKeyframe(const time_duration& t) {}
      virtual void EndKeyframeUpdate(bool cancel) {}

      string str;
      RowFlags flags;
      EffectRow* parent;
      StyledRectangle* rect;
      vector<EffectRow*> children;
      Text text;
      int level;
      FloatRect expandRect;
      FloatRect varEditRect;
      StyledRectangle* keyframeRect;
    };

    struct EffectRowNoise : public EffectRow
    {
      EffectRowNoise(
          const Font& font,
          const string& str,
          EffectRow* parent = nullptr);

      virtual void DrawVars(RenderTexture& texture);
      virtual float NumVars();
      virtual void InitVarEdit(float x, float y);
      virtual void EndEditVars(bool cancel);
      virtual void UpdateEditVar(Keyboard::Key key);
      virtual bool KeyframeIntersect(const Vector2f& pt);
      virtual void UpdateKeyframe(const time_duration& t);
      virtual void EndKeyframeUpdate(bool cancel);

      int editingIdx;

      NoiseEffector effector;
      Vector3f prevValue;
      string curEdit;
      Vector3Keyframe* selectedKeyframe;
    };

    struct TimelineFlagsF
    {
      enum Enum { PendingDrag = 1 << 0, PendingEffectMove = 1 << 1};
      struct Bits { u32 pendingDrag : 1; u32 pendingEffectMove : 1; };
    };

    typedef Flags<TimelineFlagsF> TimelineFlags;


    bool OnMouseButtonPressed(const Event& event);
    bool OnMouseMoved(const Event& event);
    bool OnMouseButtonReleased(const Event& event);
    bool OnMouseWheelMoved(const Event& event);
    bool OnKeyReleased(const Event& event);

    void RecalcEffecRows();

    template<typename T>
    sf::Vector2<T> PointToLocal(int x, int y)
    {
      return sf::Vector2<T>((T)(x - _pos.x), (T)(y - _pos.y));
    }

    time_duration _panelOffset;
    u32 _pixelsPerSecond;
    Font _font;

    TimelineFlags _timelineFlags;
    vector<EffectRow*> _effectRows;

    Vector2i _lastDragPos;
    EffectRow* _editRow;
    StyledRectangle* _tickerRect;
    EffectRow* _movingKeyframe;
  };

}