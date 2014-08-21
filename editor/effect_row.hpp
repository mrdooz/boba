#pragma once

#include "protocol/effects_proto.hpp"
#include "style_factory.hpp"

namespace editor
{
  struct StyledRectangle;

  //----------------------------------------------------------------------------------
  struct RowVar
  {
    RowVar(const Font& font, const string& name, FloatAnim* anim);

    struct VarFlagsF
    {
      enum Enum : u8 { Selected = 1 << 0, Editing = 1 << 1, Animating = 1 << 2, MovingKeyframe = 1 << 3 };
      struct Bits { u8 selected : 1; u8 editing : 1; u8 animating : 1; u32 movingKeyframe : 1; };
    };

    typedef Flags<VarFlagsF> VarFlags;

    void Draw(RenderTexture& texture);
    void DrawKeyframes(RenderTexture& texture);

    bool OnMouseButtonPressed(const Event &event);
    bool OnMouseButtonReleased(const Event &event);
    bool OnMouseMoved(const Event &event);

    Text _text;
    Font _font;
    FloatRect _bounds;

    string _name;
    FloatAnim* _anim;
    VarFlags _flags;
    StyledRect _keyframeRect;
    u32 _selectedKeyframe;
  };

  //----------------------------------------------------------------------------------
  struct EffectRow
  {
    struct RowFlagsF
    {
      enum Enum { Expanded = 1 << 0, Selected = 1 << 1, Editing = 1 << 2, MovingKeyframe = 1 << 3 };
      struct Bits { u32 expanded : 1; u32 selected : 1; u32 editing : 1; u32 movingKeyframe : 1; };
    };

    typedef Flags<RowFlagsF> RowFlags;

    EffectRow(
        const Font& font,
        const string& str,
        EffectRow* parent = nullptr);
    virtual ~EffectRow();

    void Draw(RenderTexture& texture, bool drawKeyframes);
    void Flatten(vector<EffectRow*>* res);
    void Reposition(float curY, float rowHeight);
    float RowHeight(float rowHeight);
    void DrawVars(RenderTexture& texture, bool drawKeyframes);

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
    virtual bool OnMouseMoved(const Event &event);
    virtual bool OnMouseButtonPressed(const Event &event);
    virtual bool OnMouseButtonReleased(const Event &event);
    virtual bool OnKeyReleased(const Event& event);

    virtual bool ToProtocol(google::protobuf::Message* msg) const { return true; }
    virtual bool FromProtocol(const google::protobuf::Message& msg) { return true; }
    u32 NumVars() const { return (u32)_vars.size(); }

    string _str;
    RowFlags _flags;
    EffectRow* _parent;
    StyledRect _rect;
    vector<EffectRow*> _children;
    Text _text;
    Font _font;
    int _level;
    FloatRect _expandRect;
    FloatRect _varEditRect;
    StyledRect _keyframeRect;
    u32 _id;
    vector<RowVar> _vars;
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

    virtual bool ToProtocol(google::protobuf::Message* msg) const;
    virtual bool FromProtocol(const google::protobuf::Message& msg);
    //vector<EffectRowTextPath*> _textPaths;
    //vector<EffectRowNoise*> _noise;
  };

  //----------------------------------------------------------------------------------
  struct EffectRowTextPath : public EffectRow
  {
    EffectRowTextPath(
        const Font& font,
        const string& str,
        EffectRow* parent = nullptr);

    virtual bool ToProtocol(google::protobuf::Message* msg) const;
    virtual bool FromProtocol(const google::protobuf::Message& msg);

    TextPath _textPath;
  };

  //----------------------------------------------------------------------------------
  struct EffectRowNoise : public EffectRow
  {
    EffectRowNoise(
        const Font& font,
        const string& str,
        EffectRow* parent = nullptr);

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

    virtual bool FromProtocol(const google::protobuf::Message& msg);
    virtual bool ToProtocol(google::protobuf::Message* msg) const;

    //void DrawKeyframes(RenderTexture& texture, const Vector2f& size);

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