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
  struct EffectSetting;
  struct EffectSettings;
  struct Plexus;
  struct TextPath;
  struct NoiseEffector;

  Vector3Keyframe FromProtocol(const common::protocol::Vector3Keyframe& p);
  void ToProtocol(const Vector3Keyframe& v, common::protocol::Vector3Keyframe* p);
  Vector3Anim FromProtocol(const common::protocol::Vector3Anim& p);
  void ToProtocol(const Vector3Anim& v, common::protocol::Vector3Anim* p);
  Settings FromProtocol(const editor::protocol::Settings& p);
  void ToProtocol(const Settings& v, editor::protocol::Settings* p);
  Style FromProtocol(const editor::protocol::Style& p);
  void ToProtocol(const Style& v, editor::protocol::Style* p);
  Styles FromProtocol(const editor::protocol::Styles& p);
  void ToProtocol(const Styles& v, editor::protocol::Styles* p);
  EffectSetting FromProtocol(const effect::protocol::EffectSetting& p);
  void ToProtocol(const EffectSetting& v, effect::protocol::EffectSetting* p);
  EffectSettings FromProtocol(const effect::protocol::EffectSettings& p);
  void ToProtocol(const EffectSettings& v, effect::protocol::EffectSettings* p);
  Plexus FromProtocol(const effect::protocol::plexus::Plexus& p);
  void ToProtocol(const Plexus& v, effect::protocol::plexus::Plexus* p);
  TextPath FromProtocol(const effect::protocol::plexus::TextPath& p);
  void ToProtocol(const TextPath& v, effect::protocol::plexus::TextPath* p);
  NoiseEffector FromProtocol(const effect::protocol::plexus::NoiseEffector& p);
  void ToProtocol(const NoiseEffector& v, effect::protocol::plexus::NoiseEffector* p);
  
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
  
  struct Vector3Keyframe
  {
    uint32_t time;
    Vector3f value;
  };
  struct Vector3Anim
  {
    uint32_t type;
    vector<Vector3Keyframe> keyframes;
  };
  struct Settings
  {
    uint32_t tickerHeight;
    uint32_t tickerInterval;
    uint32_t ticksPerInterval;
    uint32_t effectViewWidth;
    uint32_t effectRowHeight;
    uint32_t statusBarHeight;
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
  struct Style
  {
    string id;
    Color fillColor;
    Color outlineColor;
    float outlineThickness;
    uint32_t fontStyle;
  };
  struct Styles
  {
    vector<Style> styles;
  };
  struct EffectSetting
  {
    enum class Type
    {
      Plexus = 1,
    };

    EffectSetting::Type type;
    std::vector<uint8_t> msg;
  };
  struct EffectSettings
  {
    vector<EffectSetting> effectSettings;
  };
  struct Plexus
  {
    vector<TextPath> textPaths;
    vector<NoiseEffector> noiseEffectors;
  };
  struct TextPath
  {
    string text;
  };
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
  

  inline Color FromProtocol(const common::protocol::Color4& col)
  {
    return Color(col.r(), col.g(), col.b(), col.a());
  }

  inline void ToProtocol(const Color& v, common::protocol::Color4* p)
  {
    p->set_r(v.r);
    p->set_g(v.g);
    p->set_b(v.b);
    p->set_a(v.a);
  }

  inline Vector3f FromProtocol(const common::protocol::Vector3& v)
  {
    return Vector3f(v.x(), v.y(), v.z());
  }

  inline void ToProtocol(const Vector3f& v, common::protocol::Vector3* p)
  {
    p->set_x(v.x);
    p->set_y(v.y);
    p->set_z(v.z);
  }


  inline Vector3Keyframe FromProtocol(const common::protocol::Vector3Keyframe& p)
  {
    Vector3Keyframe res;
    res.time = p.time();
    res.value = FromProtocol(p.value());
    return res;
  }

  inline void ToProtocol(const Vector3Keyframe& v, common::protocol::Vector3Keyframe* p)
  {
    p->set_time(v.time);
    ToProtocol(v.value, p->mutable_value());
  }


  inline Vector3Anim FromProtocol(const common::protocol::Vector3Anim& p)
  {
    Vector3Anim res;
    res.type = p.type();
    res.keyframes = FromProtocolRepeated<Vector3Keyframe>(p.keyframes());
    return res;
  }

  inline void ToProtocol(const Vector3Anim& v, common::protocol::Vector3Anim* p)
  {
    p->set_type(v.type);
    for (const auto& x : v.keyframes)
      ToProtocol(x, p->add_keyframes());
  }


  inline Settings FromProtocol(const editor::protocol::Settings& p)
  {
    Settings res;
    res.tickerHeight = p.ticker_height();
    res.tickerInterval = p.ticker_interval();
    res.ticksPerInterval = p.ticks_per_interval();
    res.effectViewWidth = p.effect_view_width();
    res.effectRowHeight = p.effect_row_height();
    res.statusBarHeight = p.status_bar_height();
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

  inline void ToProtocol(const Settings& v, editor::protocol::Settings* p)
  {
    p->set_ticker_height(v.tickerHeight);
    p->set_ticker_interval(v.tickerInterval);
    p->set_ticks_per_interval(v.ticksPerInterval);
    p->set_effect_view_width(v.effectViewWidth);
    p->set_effect_row_height(v.effectRowHeight);
    p->set_status_bar_height(v.statusBarHeight);
    p->set_effect_height(v.effectHeight);
    p->set_resize_handle(v.resizeHandle);
    p->set_timeline_zoom_min(v.timelineZoomMin);
    p->set_timeline_zoom_max(v.timelineZoomMax);
    p->set_timeline_zoom_default(v.timelineZoomDefault);
    ToProtocol(v.defaultRowColor, p->mutable_default_row_color());
    ToProtocol(v.selectedRowColor, p->mutable_selected_row_color());
    ToProtocol(v.hoverRowColor, p->mutable_hover_row_color());
    ToProtocol(v.invalidHoverRowColor, p->mutable_invalid_hover_row_color());
    ToProtocol(v.effectIconExpandedColor, p->mutable_effect_icon_expanded_color());
    ToProtocol(v.effectIconCollapsedColor, p->mutable_effect_icon_collapsed_color());
  }


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

  inline void ToProtocol(const Style& v, editor::protocol::Style* p)
  {
    p->set_id(v.id);
    ToProtocol(v.fillColor, p->mutable_fill_color());
    ToProtocol(v.outlineColor, p->mutable_outline_color());
    p->set_outline_thickness(v.outlineThickness);
    p->set_font_style(v.fontStyle);
  }


  inline Styles FromProtocol(const editor::protocol::Styles& p)
  {
    Styles res;
    res.styles = FromProtocolRepeated<Style>(p.styles());
    return res;
  }

  inline void ToProtocol(const Styles& v, editor::protocol::Styles* p)
  {
    for (const auto& x : v.styles)
      ToProtocol(x, p->add_styles());
  }


  inline EffectSetting FromProtocol(const effect::protocol::EffectSetting& p)
  {
    EffectSetting res;
    res.type = (EffectSetting::Type)p.type();
    res.msg.resize(p.msg().size());
    memcpy(res.msg.data(), p.msg().data(), p.msg().size());
    return res;
  }

  inline void ToProtocol(const EffectSetting& v, effect::protocol::EffectSetting* p)
  {
  }


  inline EffectSettings FromProtocol(const effect::protocol::EffectSettings& p)
  {
    EffectSettings res;
    res.effectSettings = FromProtocolRepeated<EffectSetting>(p.effect_settings());
    return res;
  }

  inline void ToProtocol(const EffectSettings& v, effect::protocol::EffectSettings* p)
  {
    for (const auto& x : v.effectSettings)
      ToProtocol(x, p->add_effect_settings());
  }


  inline Plexus FromProtocol(const effect::protocol::plexus::Plexus& p)
  {
    Plexus res;
    res.textPaths = FromProtocolRepeated<TextPath>(p.text_paths());
    res.noiseEffectors = FromProtocolRepeated<NoiseEffector>(p.noise_effectors());
    return res;
  }

  inline void ToProtocol(const Plexus& v, effect::protocol::plexus::Plexus* p)
  {
    for (const auto& x : v.textPaths)
      ToProtocol(x, p->add_text_paths());
    for (const auto& x : v.noiseEffectors)
      ToProtocol(x, p->add_noise_effectors());
  }


  inline TextPath FromProtocol(const effect::protocol::plexus::TextPath& p)
  {
    TextPath res;
    res.text = p.text();
    return res;
  }

  inline void ToProtocol(const TextPath& v, effect::protocol::plexus::TextPath* p)
  {
    p->set_text(v.text);
  }


  inline NoiseEffector FromProtocol(const effect::protocol::plexus::NoiseEffector& p)
  {
    NoiseEffector res;
    res.applyTo = (NoiseEffector::ApplyTo)p.apply_to();
    res.displacement = FromProtocol(p.displacement());
    return res;
  }

  inline void ToProtocol(const NoiseEffector& v, effect::protocol::plexus::NoiseEffector* p)
  {
    ToProtocol(v.displacement, p->mutable_displacement());
  }

	
}
