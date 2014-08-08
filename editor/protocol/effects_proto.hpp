#pragma once

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable: 4244 4267)
#endif

#include "common.pb.h"
#include "editor_settings.pb.h"
#include "effect_settings.pb.h"
#include "effect_settings_plexus.pb.h"
#include "editor_styles.pb.h"

#ifdef _WIN32
#pragma warning(pop)
#endif

namespace editor
{
  struct Vector2;
  struct Vector3;
  struct Vector4;
  struct Vector3Keyframe;
  struct Vector3Anim;
  struct Settings;
  struct Style;
  struct Styles;
  struct Plexus;
  struct TextPath;
  struct NoiseEffector;

  Vector3Keyframe FromProtocol(const common::protocol::Vector3Keyframe& p);
  Vector3Anim FromProtocol(const common::protocol::Vector3Anim& p);
  Settings FromProtocol(const editor::protocol::Settings& p);
  Style FromProtocol(const editor::protocol::Style& p);
  Styles FromProtocol(const editor::protocol::Styles& p);
  Plexus FromProtocol(const effect::plexus::Plexus& p);
  TextPath FromProtocol(const effect::plexus::TextPath& p);
  NoiseEffector FromProtocol(const effect::plexus::NoiseEffector& p);
  
  template<typename T, typename U>
  vector<T> FromProtocolRepeated(const google::protobuf::RepeatedPtrField<U>& v)
  {
    vector<T> res;
    for (const auto& x : v)
    {
      res.push_back(FromProtocol(x));
    }
    return res;
  }
  
  inline Color FromProtocol(const editor::protocol::Color4& col)
  {
    return Color(col.r(), col.g(), col.b(), col.a());
  }

  inline Vector3f FromProtocol(const common::protocol::Vector3& v)
  {
    return Vector3f(v.x(), v.y(), v.z());
  }

  struct Vector3Keyframe
  {
    uint32_t time;
    Vector3f value;
  };

  inline Vector3Keyframe FromProtocol(const common::protocol::Vector3Keyframe& p)
  {
    Vector3Keyframe res;
    res.time = p.time();
    res.value = FromProtocol(p.value());
    return res;
  }

  struct Vector3Anim
  {
    uint32_t type;
    vector<Vector3Keyframe> keyframes;
  };

  inline Vector3Anim FromProtocol(const common::protocol::Vector3Anim& p)
  {
    Vector3Anim res;
    res.type = p.type();
    res.keyframes = FromProtocolRepeated<Vector3Keyframe>(p.keyframes());
    return res;
  }

  struct Settings
  {
    uint32_t tickerHeight;
    uint32_t tickerInterval;
    uint32_t ticksPerInterval;
    uint32_t effectViewWidth;
    uint32_t effectRowHeight;
    uint32_t effectHeight;
    uint32_t resizeHandle;
    uint32_t timelineZoomMin;
    uint32_t timelineZoomMax;
    uint32_t timelineZoomDefault;
    Color defaultRowColor;
    Color selectedRowColor;
    Color hoverRowColor;
    Color invalidHoverRowColor;
    Color effectIconExpandedColor;
    Color effectIconCollapsedColor;
  };

  inline Settings FromProtocol(const editor::protocol::Settings& p)
  {
    Settings res;
    res.tickerHeight = p.ticker_height();
    res.tickerInterval = p.ticker_interval();
    res.ticksPerInterval = p.ticks_per_interval();
    res.effectViewWidth = p.effect_view_width();
    res.effectRowHeight = p.effect_row_height();
    res.effectHeight = p.effect_height();
    res.resizeHandle = p.resize_handle();
    res.timelineZoomMin = p.timeline_zoom_min();
    res.timelineZoomMax = p.timeline_zoom_max();
    res.timelineZoomDefault = p.timeline_zoom_default();
    res.defaultRowColor = FromProtocol(p.default_row_color());
    res.selectedRowColor = FromProtocol(p.selected_row_color());
    res.hoverRowColor = FromProtocol(p.hover_row_color());
    res.invalidHoverRowColor = FromProtocol(p.invalid_hover_row_color());
    res.effectIconExpandedColor = FromProtocol(p.effect_icon_expanded_color());
    res.effectIconCollapsedColor = FromProtocol(p.effect_icon_collapsed_color());
    return res;
  }

  struct Style
  {
    string id;
    Color fillColor;
    Color outlineColor;
    float outlineThickness;
    int32_t fontStyle;
  };

  inline Style FromProtocol(const editor::protocol::Style& p)
  {
    Style res;
    res.id = p.id();
    res.fillColor = FromProtocol(p.fill_color());
    res.outlineColor = FromProtocol(p.outline_color());
    res.outlineThickness = p.outline_thickness();
    res.fontStyle = p.font_style();
    return res;
  }

  struct Styles
  {
    vector<Style> styles;
  };

  inline Styles FromProtocol(const editor::protocol::Styles& p)
  {
    Styles res;
    res.styles = FromProtocolRepeated<Style>(p.styles());
    return res;
  }

  struct Plexus
  {
    vector<TextPath> textPaths;
    vector<NoiseEffector> noiseEffectors;
  };

  inline Plexus FromProtocol(const effect::plexus::Plexus& p)
  {
    Plexus res;
    res.textPaths = FromProtocolRepeated<TextPath>(p.text_paths());
    res.noiseEffectors = FromProtocolRepeated<NoiseEffector>(p.noise_effectors());
    return res;
  }

  struct TextPath
  {
    string text;
  };

  inline TextPath FromProtocol(const effect::plexus::TextPath& p)
  {
    TextPath res;
    res.text = p.text();
    return res;
  }

  struct NoiseEffector
  {
    enum class ApplyTo
    {
      Position = 1,
      Scale = 2,
    };

    NoiseEffector::ApplyTo applyTo;
    Vector3Anim displacement;
  };

  inline NoiseEffector FromProtocol(const effect::plexus::NoiseEffector& p)
  {
    NoiseEffector res;
    res.applyTo = (NoiseEffector::ApplyTo)p.apply_to();
    res.displacement = FromProtocol(p.displacement());
    return res;
  }

	
}
