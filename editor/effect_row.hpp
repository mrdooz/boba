#pragma once

#include "protocol/effect_types.hpp"
#include "protocol/effect_plexus_types.hpp"
#include "style_factory.hpp"

namespace editor
{
  struct StyledRectangle;
  struct RowVar;
  struct EffectRowEvent;

  //----------------------------------------------------------------------------------
  struct EffectRow
  {
    struct RowFlagsF
    {
      enum Enum { Expanded = 1 << 0 };
      struct Bits { u32 expanded : 1; };
    };

    typedef Flags<RowFlagsF> RowFlags;

    EffectRow(const Font& font, const string& str, EffectRow* parent = nullptr);
    virtual ~EffectRow();

    void Draw(RenderTexture& texture, bool drawKeyframes);
    void Flatten(vector<EffectRow*>* res);
    void Reposition(float curY, float rowHeight);
    float RowHeight(float rowHeight);
    void DrawVars(RenderTexture& texture, bool drawKeyframes);

    virtual bool OnMouseMoved(const Event &event);
    virtual bool OnMouseButtonPressed(const Event &event);
    virtual bool OnMouseButtonReleased(const Event &event);
    virtual bool OnKeyReleased(const Event& event);

    virtual bool ToProtocol(google::protobuf::Message* msg) const { return true; }
    virtual bool FromProtocol(const google::protobuf::Message& msg) { return true; }
    u32 NumVars() const { return (u32)_vars.size(); }
    void SendEvent(RowVar* sender, const EffectRowEvent& event);

    string _str;
    RowFlags _flags;
    EffectRow* _parent;
    StyledRect _rect;
    vector<EffectRow*> _children;
    Text _text;
    Font _font;
    int _level;
    FloatRect _expandRect;
    StyledRect _keyframeRect;
    u32 _id;
    vector<RowVar*> _vars;
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

    virtual bool FromProtocol(const google::protobuf::Message& msg);
    virtual bool ToProtocol(google::protobuf::Message* msg) const;

    NoiseEffector _effector;
  };

}