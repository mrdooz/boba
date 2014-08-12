#pragma once

#include "protocol/effects_proto.hpp"

namespace editor
{
  struct StyledRectangle;

  //----------------------------------------------------------------------------------
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
    virtual bool OnMouseMoved(const Event &event) { return true; }
    virtual bool OnMouseButtonPressed(const Event &event) { return true; }
    virtual bool OnMouseButtonReleased(const Event &event) { return true; }

    virtual bool ToProtocol(effect::protocol::EffectSetting* proto) const { return true; }

    string _str;
    RowFlags _flags;
    EffectRow* _parent;
    StyledRectangle* _rect;
    vector<EffectRow*> _children;
    Text _text;
    int _level;
    FloatRect _expandRect;
    FloatRect _varEditRect;
    StyledRectangle*_keyframeRect;
    Font _font;
  };


  struct EffectRowTextPath;
  struct EffectRowNoise;
  //----------------------------------------------------------------------------------
  struct EffectRowPlexus : public EffectRow
  {
    EffectRowPlexus(
        const Font& font,
        const string& str,
        EffectRow* parent = nullptr);

    virtual bool ToProtocol(effect::protocol::EffectSetting* proto) const;
    vector<EffectRowTextPath*> _textPaths;
    vector<EffectRowNoise*> _noise;
  };

  //----------------------------------------------------------------------------------
  struct EffectRowTextPath : public EffectRow
  {
    EffectRowTextPath(
        const Font& font,
        const string& str,
        EffectRow* parent = nullptr);

    bool ToProtocolInner(effect::protocol::plexus::Plexus* proto) const;
  };

  //----------------------------------------------------------------------------------
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
    virtual void DrawGraph(RenderTexture& texture, const Vector2f& size);
    virtual bool OnMouseMoved(const Event &event);
    virtual bool OnMouseButtonPressed(const Event &event);
    virtual bool OnMouseButtonReleased(const Event &event);

    bool ToProtocolInner(effect::protocol::plexus::Plexus* proto) const;

    void DrawKeyframes(RenderTexture& texture, const Vector2f& size);

    void VisibleKeyframes(
        const Vector2f& size,
        bool addBorderPoints,
        vector<pair<Vector2f, Vector3Keyframe*>>* keyframes);

    float CalcGraphValue(const Vector3f& value) const;
    float ExtractGraphValue(const Vector3f& value) const;
    float& ExtractGraphValue(Vector3f& value);
    Vector3f PixelToValue(int x) const;
    int ValueToPixel(const Vector3f& v);
    Vector3f UpdateKeyframe(const Vector3f& newValue, const Vector3f& old) const;

    void CalcCeilAndStep(float value, float* stepValue, float* ceilValue);
    void CalcCeilAndStep(const Vector3f& value, Vector3f* stepValue, Vector3f* ceilValue);
    float SnappedValue(float value);

    int _editingIdx;

    NoiseEffector _effector;
    Vector3f _prevValue;
    string _curEdit;
    Vector3Keyframe*_selectedKeyframe;
    Vector3Keyframe _oldKeyframe;
    bool _copyingKeyframe;

    u32 _graphMode;
    Vector3f _minValue, _maxValue;
    Vector3f _realMinValue, _realMaxValue;
  };

}