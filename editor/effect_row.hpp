#pragma once

#include "protocol/effects_proto.hpp"

namespace editor
{
  struct StyledRectangle;

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

    void Draw(RenderTexture& texture, bool drawKeyframes);
    static void Flatten(EffectRow* cur, vector<EffectRow*>* res);
    static void Reposition(EffectRow* cur, float curY, float rowHeight);
    static float RowHeight(EffectRow* cur, float rowHeight);
    virtual void DrawVars(RenderTexture& texture, bool drawKeyframes) {}
    virtual u32 NumVars() { return 0; }
    virtual void BeginEditVars(float x, float y) {}
    virtual void UpdateEditVar(Keyboard::Key key) {}
    virtual void EndEditVars(bool commit) {}

    virtual bool KeyframeIntersect(const Vector2f& pt, const Vector2f& size) { return false; }
    virtual void BeginKeyframeUpdate(bool copy) {}
    virtual void UpdateKeyframe(const time_duration& t) {}
    virtual void EndKeyframeUpdate(bool commit) {}
    virtual void DeselectKeyframe() {}
    virtual void DeleteKeyframe() {}

    virtual void ToggleGraphView(bool value) {}
    // return true if cycled through all graphs
    virtual bool NextGraph() { return true; }
    virtual void DrawGraph(RenderTexture& texture, const Vector2f& size) {}
    virtual bool GraphMouseMoved(const Event& event) { return true; }
    virtual bool GraphMouseButtonPressed(const Event& event) { return true; }
    virtual bool GraphMouseButtonReleased(const Event& event) { return true; }


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
    Font font;
  };

  struct EffectRowNoise : public EffectRow
  {
    EffectRowNoise(
        const Font& font,
        const string& str,
        EffectRow* parent = nullptr);

    virtual void DrawVars(RenderTexture& texture, bool drawKeyframes);
    virtual u32 NumVars();
    virtual void BeginEditVars(float x, float y);
    virtual void EndEditVars(bool commit);
    virtual void UpdateEditVar(Keyboard::Key key);

    virtual bool KeyframeIntersect(const Vector2f& pt, const Vector2f& size);
    virtual void BeginKeyframeUpdate(bool copy);
    virtual void UpdateKeyframe(const time_duration& t);
    virtual void EndKeyframeUpdate(bool commit);
    virtual void DeselectKeyframe();
    virtual void DeleteKeyframe();

    virtual void ToggleGraphView(bool value);
    virtual bool NextGraph();
    virtual bool GraphMouseMoved(const Event& event);
    virtual bool GraphMouseButtonPressed(const Event& event);
    virtual bool GraphMouseButtonReleased(const Event& event);
    virtual void DrawGraph(RenderTexture& texture, const Vector2f& size);

    void DrawKeyframes(RenderTexture& texture, const Vector2f& size);

    void VisibleKeyframes(
        const Vector2f& size,
        bool addBorderPoints,
        vector<pair<Vector2f, Vector3Keyframe*>>* keyframes);

    float CalcGraphValue(const Vector3f& value) const;
    float ExtractGraphValue(const Vector3f& value) const;
    Vector3f PixelToValue(int y) const;
    Vector3f UpdateKeyframe(const Vector3f& newValue, const Vector3f& old) const;

    int editingIdx;

    NoiseEffector effector;
    Vector3f prevValue;
    string curEdit;
    Vector3Keyframe* selectedKeyframe;
    Vector3Keyframe oldKeyframe;
    bool copyingKeyframe;

    u32 graphMode;
    Vector3f minValue, maxValue;
    Vector3f realMinValue, realMaxValue;
  };

}