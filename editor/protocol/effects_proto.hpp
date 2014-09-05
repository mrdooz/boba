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
#include <bristol/flags.hpp>

#ifdef _WIN32
#pragma warning(pop)
#endif

namespace editor
{
  struct Vector2;
  struct Vector3;
  struct Vector4;
  struct FloatKey;
  struct FloatKeyframe;
  struct FloatAnim;
  struct Vector3Keyframe;
  struct Vector3Anim;
  struct Settings;
  struct StyleSetting;
  struct StyleSettings;
  struct Plexus;
  struct TextPath;
  struct Displacement;
  struct NoiseEffector;
  struct EffectSetting;
  struct EffectSettings;

  FloatKey FromProtocol(const common::protocol::FloatKey& p);
  void ToProtocol(const FloatKey& v, common::protocol::FloatKey* p);
  FloatKeyframe FromProtocol(const common::protocol::FloatKeyframe& p);
  void ToProtocol(const FloatKeyframe& v, common::protocol::FloatKeyframe* p);
  FloatAnim FromProtocol(const common::protocol::FloatAnim& p);
  void ToProtocol(const FloatAnim& v, common::protocol::FloatAnim* p);
  Vector3Keyframe FromProtocol(const common::protocol::Vector3Keyframe& p);
  void ToProtocol(const Vector3Keyframe& v, common::protocol::Vector3Keyframe* p);
  Vector3Anim FromProtocol(const common::protocol::Vector3Anim& p);
  void ToProtocol(const Vector3Anim& v, common::protocol::Vector3Anim* p);
  Settings FromProtocol(const editor::protocol::Settings& p);
  void ToProtocol(const Settings& v, editor::protocol::Settings* p);
  StyleSetting FromProtocol(const editor::protocol::StyleSetting& p);
  void ToProtocol(const StyleSetting& v, editor::protocol::StyleSetting* p);
  StyleSettings FromProtocol(const editor::protocol::StyleSettings& p);
  void ToProtocol(const StyleSettings& v, editor::protocol::StyleSettings* p);
  Plexus FromProtocol(const effect::protocol::plexus::Plexus& p);
  void ToProtocol(const Plexus& v, effect::protocol::plexus::Plexus* p);
  TextPath FromProtocol(const effect::protocol::plexus::TextPath& p);
  void ToProtocol(const TextPath& v, effect::protocol::plexus::TextPath* p);
  Displacement FromProtocol(const effect::protocol::plexus::Displacement& p);
  void ToProtocol(const Displacement& v, effect::protocol::plexus::Displacement* p);
  NoiseEffector FromProtocol(const effect::protocol::plexus::NoiseEffector& p);
  void ToProtocol(const NoiseEffector& v, effect::protocol::plexus::NoiseEffector* p);
  EffectSetting FromProtocol(const effect::protocol::EffectSetting& p);
  void ToProtocol(const EffectSetting& v, effect::protocol::EffectSetting* p);
  EffectSettings FromProtocol(const effect::protocol::EffectSettings& p);
  void ToProtocol(const EffectSettings& v, effect::protocol::EffectSettings* p);
  
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
  
  struct FloatKey
  {
    int64_t time;
    float value;
  };

  struct FloatKeyframe
  {
    FloatKey key;
    FloatKey cpIn;
    FloatKey cpOut;
  };

  struct FloatAnim
  {
    uint32_t type = 1;
    vector<FloatKeyframe> keyframe;
    float minValue;
    float maxValue;
    float step;
  };

  struct Vector3Keyframe
  {
    int32_t time;
    Vector3f value;
  };

  struct Vector3Anim
  {
    uint32_t type;
    vector<Vector3Keyframe> keyframe;
    Vector3f minValue;
    Vector3f maxValue;
    Vector3f step;
  };

  struct Settings
  {
    uint32_t tickerHeight = 50;
    uint32_t tickerInterval = 20;
    uint32_t ticksPerInterval = 4;
    uint32_t effectViewWidth = 200;
    uint32_t effectRowHeight = 20;
    uint32_t statusBarHeight = 30;
    uint32_t effectHeight = 40;
    uint32_t resizeHandle = 6;
    uint32_t timelineZoomMin = 16;
    uint32_t timelineZoomMax = 1024;
    uint32_t timelineZoomDefault = 128;
    uint32_t keyframeSize = 5;
    Color defaultRowColor;
    Color selectedRowColor;
    Color hoverRowColor;
    Color invalidHoverRowColor;
    Color defaultLineColor;
    Color effectViewBackgroundColor;
    Color effectViewBackgroundColorSelected;
    Color timelineViewBackgroundColor;
    Color effectLineColor;
    Color keyframeDefaultColor;
    Color keyframeSelectedColor;
    Color keyframeControlColor;
    Color graphColor;
    Color varTextColor;
    Color varValueColor;
    Color effectIconExpandedColor;
    Color effectIconCollapsedColor;
  };

  struct StyleSetting
  {
    string id;
    Color fillColor;
    Color outlineColor;
    float outlineThickness = 0;
    uint32_t fontStyle;
  };

  struct StyleSettings
  {
    vector<StyleSetting> styleSetting;
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

  struct Displacement
  {
    FloatAnim x;
    FloatAnim y;
    FloatAnim z;
  };

  struct NoiseEffector
  {
    enum class ApplyTo
    {
      Position = 1,
      Scale = 2,
    };

    NoiseEffector::ApplyTo applyTo = ApplyTo::Position;
    Displacement displacement;
  };

  struct EffectSetting
  {
    enum class Type
    {
      Unknown = 0,
      Plexus = 1,
    };

    EffectSetting::Type type;
    string effectClass;
    uint32_t id;
    string name;
    uint32_t startTime;
    uint32_t endTime;
    std::vector<uint8_t> configMsg;
    string configFile;
  };

  struct EffectSettings
  {
    vector<EffectSetting> effectSetting;
    string soundtrack;
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

  inline Vector2f FromProtocol(const common::protocol::Vector2& v)
  {
    return Vector2f(v.x(), v.y());
  }

  inline void ToProtocol(const Vector2f& v, common::protocol::Vector2* p)
  {
    p->set_x(v.x);
    p->set_y(v.y);
  }

  inline FloatKey FromProtocol(const common::protocol::FloatKey& p)
  {
    FloatKey res;
    if (p.has_time())
    {
      res.time = p.time();
    }
    if (p.has_value())
    {
      res.value = p.value();
    }
    return res;
  }

  inline void ToProtocol(const FloatKey& v, common::protocol::FloatKey* p)
  {
    p->set_time(v.time);
    p->set_value(v.value);
  }

  inline FloatKeyframe FromProtocol(const common::protocol::FloatKeyframe& p)
  {
    FloatKeyframe res;
    if (p.has_key())
    {
      res.key = FromProtocol(p.key());
    }
    if (p.has_cp_in())
    {
      res.cpIn = FromProtocol(p.cp_in());
    }
    if (p.has_cp_out())
    {
      res.cpOut = FromProtocol(p.cp_out());
    }
    return res;
  }

  inline void ToProtocol(const FloatKeyframe& v, common::protocol::FloatKeyframe* p)
  {
    ToProtocol(v.key, p->mutable_key());
    ToProtocol(v.cpIn, p->mutable_cp_in());
    ToProtocol(v.cpOut, p->mutable_cp_out());
  }

  inline FloatAnim FromProtocol(const common::protocol::FloatAnim& p)
  {
    FloatAnim res;
    if (p.has_type())
    {
      res.type = p.type();
    }
    res.keyframe = FromProtocolRepeated<FloatKeyframe>(p.keyframe());
    if (p.has_min_value())
    {
      res.minValue = p.min_value();
    }
    if (p.has_max_value())
    {
      res.maxValue = p.max_value();
    }
    if (p.has_step())
    {
      res.step = p.step();
    }
    return res;
  }

  inline void ToProtocol(const FloatAnim& v, common::protocol::FloatAnim* p)
  {
    p->set_type(v.type);
    for (const auto& x : v.keyframe)
      ToProtocol(x, p->add_keyframe());
    p->set_min_value(v.minValue);
    p->set_max_value(v.maxValue);
    p->set_step(v.step);
  }

  inline Vector3Keyframe FromProtocol(const common::protocol::Vector3Keyframe& p)
  {
    Vector3Keyframe res;
    if (p.has_time())
    {
      res.time = p.time();
    }
    if (p.has_value())
    {
      res.value = FromProtocol(p.value());
    }
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
    if (p.has_type())
    {
      res.type = p.type();
    }
    res.keyframe = FromProtocolRepeated<Vector3Keyframe>(p.keyframe());
    if (p.has_min_value())
    {
      res.minValue = FromProtocol(p.min_value());
    }
    if (p.has_max_value())
    {
      res.maxValue = FromProtocol(p.max_value());
    }
    if (p.has_step())
    {
      res.step = FromProtocol(p.step());
    }
    return res;
  }

  inline void ToProtocol(const Vector3Anim& v, common::protocol::Vector3Anim* p)
  {
    p->set_type(v.type);
    for (const auto& x : v.keyframe)
      ToProtocol(x, p->add_keyframe());
    ToProtocol(v.minValue, p->mutable_min_value());
    ToProtocol(v.maxValue, p->mutable_max_value());
    ToProtocol(v.step, p->mutable_step());
  }

  inline Settings FromProtocol(const editor::protocol::Settings& p)
  {
    Settings res;
    if (p.has_ticker_height())
    {
      res.tickerHeight = p.ticker_height();
    }
    if (p.has_ticker_interval())
    {
      res.tickerInterval = p.ticker_interval();
    }
    if (p.has_ticks_per_interval())
    {
      res.ticksPerInterval = p.ticks_per_interval();
    }
    if (p.has_effect_view_width())
    {
      res.effectViewWidth = p.effect_view_width();
    }
    if (p.has_effect_row_height())
    {
      res.effectRowHeight = p.effect_row_height();
    }
    if (p.has_status_bar_height())
    {
      res.statusBarHeight = p.status_bar_height();
    }
    if (p.has_effect_height())
    {
      res.effectHeight = p.effect_height();
    }
    if (p.has_resize_handle())
    {
      res.resizeHandle = p.resize_handle();
    }
    if (p.has_timeline_zoom_min())
    {
      res.timelineZoomMin = p.timeline_zoom_min();
    }
    if (p.has_timeline_zoom_max())
    {
      res.timelineZoomMax = p.timeline_zoom_max();
    }
    if (p.has_timeline_zoom_default())
    {
      res.timelineZoomDefault = p.timeline_zoom_default();
    }
    if (p.has_keyframe_size())
    {
      res.keyframeSize = p.keyframe_size();
    }
    if (p.has_default_row_color())
    {
      res.defaultRowColor = FromProtocol(p.default_row_color());
    }
    if (p.has_selected_row_color())
    {
      res.selectedRowColor = FromProtocol(p.selected_row_color());
    }
    if (p.has_hover_row_color())
    {
      res.hoverRowColor = FromProtocol(p.hover_row_color());
    }
    if (p.has_invalid_hover_row_color())
    {
      res.invalidHoverRowColor = FromProtocol(p.invalid_hover_row_color());
    }
    if (p.has_default_line_color())
    {
      res.defaultLineColor = FromProtocol(p.default_line_color());
    }
    if (p.has_effect_view_background_color())
    {
      res.effectViewBackgroundColor = FromProtocol(p.effect_view_background_color());
    }
    if (p.has_effect_view_background_color_selected())
    {
      res.effectViewBackgroundColorSelected = FromProtocol(p.effect_view_background_color_selected());
    }
    if (p.has_timeline_view_background_color())
    {
      res.timelineViewBackgroundColor = FromProtocol(p.timeline_view_background_color());
    }
    if (p.has_effect_line_color())
    {
      res.effectLineColor = FromProtocol(p.effect_line_color());
    }
    if (p.has_keyframe_default_color())
    {
      res.keyframeDefaultColor = FromProtocol(p.keyframe_default_color());
    }
    if (p.has_keyframe_selected_color())
    {
      res.keyframeSelectedColor = FromProtocol(p.keyframe_selected_color());
    }
    if (p.has_keyframe_control_color())
    {
      res.keyframeControlColor = FromProtocol(p.keyframe_control_color());
    }
    if (p.has_graph_color())
    {
      res.graphColor = FromProtocol(p.graph_color());
    }
    if (p.has_var_text_color())
    {
      res.varTextColor = FromProtocol(p.var_text_color());
    }
    if (p.has_var_value_color())
    {
      res.varValueColor = FromProtocol(p.var_value_color());
    }
    if (p.has_effect_icon_expanded_color())
    {
      res.effectIconExpandedColor = FromProtocol(p.effect_icon_expanded_color());
    }
    if (p.has_effect_icon_collapsed_color())
    {
      res.effectIconCollapsedColor = FromProtocol(p.effect_icon_collapsed_color());
    }
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
    p->set_keyframe_size(v.keyframeSize);
    ToProtocol(v.defaultRowColor, p->mutable_default_row_color());
    ToProtocol(v.selectedRowColor, p->mutable_selected_row_color());
    ToProtocol(v.hoverRowColor, p->mutable_hover_row_color());
    ToProtocol(v.invalidHoverRowColor, p->mutable_invalid_hover_row_color());
    ToProtocol(v.defaultLineColor, p->mutable_default_line_color());
    ToProtocol(v.effectViewBackgroundColor, p->mutable_effect_view_background_color());
    ToProtocol(v.effectViewBackgroundColorSelected, p->mutable_effect_view_background_color_selected());
    ToProtocol(v.timelineViewBackgroundColor, p->mutable_timeline_view_background_color());
    ToProtocol(v.effectLineColor, p->mutable_effect_line_color());
    ToProtocol(v.keyframeDefaultColor, p->mutable_keyframe_default_color());
    ToProtocol(v.keyframeSelectedColor, p->mutable_keyframe_selected_color());
    ToProtocol(v.keyframeControlColor, p->mutable_keyframe_control_color());
    ToProtocol(v.graphColor, p->mutable_graph_color());
    ToProtocol(v.varTextColor, p->mutable_var_text_color());
    ToProtocol(v.varValueColor, p->mutable_var_value_color());
    ToProtocol(v.effectIconExpandedColor, p->mutable_effect_icon_expanded_color());
    ToProtocol(v.effectIconCollapsedColor, p->mutable_effect_icon_collapsed_color());
  }

  inline StyleSetting FromProtocol(const editor::protocol::StyleSetting& p)
  {
    StyleSetting res;
    if (p.has_id())
    {
      res.id = p.id();
    }
    if (p.has_fill_color())
    {
      res.fillColor = FromProtocol(p.fill_color());
    }
    if (p.has_outline_color())
    {
      res.outlineColor = FromProtocol(p.outline_color());
    }
    if (p.has_outline_thickness())
    {
      res.outlineThickness = p.outline_thickness();
    }
    if (p.has_font_style())
    {
      res.fontStyle = p.font_style();
    }
    return res;
  }

  inline void ToProtocol(const StyleSetting& v, editor::protocol::StyleSetting* p)
  {
    p->set_id(v.id);
    ToProtocol(v.fillColor, p->mutable_fill_color());
    ToProtocol(v.outlineColor, p->mutable_outline_color());
    p->set_outline_thickness(v.outlineThickness);
    p->set_font_style(v.fontStyle);
  }

  inline StyleSettings FromProtocol(const editor::protocol::StyleSettings& p)
  {
    StyleSettings res;
    res.styleSetting = FromProtocolRepeated<StyleSetting>(p.style_setting());
    return res;
  }

  inline void ToProtocol(const StyleSettings& v, editor::protocol::StyleSettings* p)
  {
    for (const auto& x : v.styleSetting)
      ToProtocol(x, p->add_style_setting());
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
    if (p.has_text())
    {
      res.text = p.text();
    }
    return res;
  }

  inline void ToProtocol(const TextPath& v, effect::protocol::plexus::TextPath* p)
  {
    p->set_text(v.text);
  }

  inline Displacement FromProtocol(const effect::protocol::plexus::Displacement& p)
  {
    Displacement res;
    if (p.has_x())
    {
      res.x = FromProtocol(p.x());
    }
    if (p.has_y())
    {
      res.y = FromProtocol(p.y());
    }
    if (p.has_z())
    {
      res.z = FromProtocol(p.z());
    }
    return res;
  }

  inline void ToProtocol(const Displacement& v, effect::protocol::plexus::Displacement* p)
  {
    ToProtocol(v.x, p->mutable_x());
    ToProtocol(v.y, p->mutable_y());
    ToProtocol(v.z, p->mutable_z());
  }

  inline NoiseEffector FromProtocol(const effect::protocol::plexus::NoiseEffector& p)
  {
    NoiseEffector res;
    res.applyTo = (NoiseEffector::ApplyTo)p.apply_to();
    if (p.has_displacement())
    {
      res.displacement = FromProtocol(p.displacement());
    }
    return res;
  }

  inline void ToProtocol(const NoiseEffector& v, effect::protocol::plexus::NoiseEffector* p)
  {
    ToProtocol(v.displacement, p->mutable_displacement());
  }

  inline EffectSetting FromProtocol(const effect::protocol::EffectSetting& p)
  {
    EffectSetting res;
    res.type = (EffectSetting::Type)p.type();
    if (p.has_effect_class())
    {
      res.effectClass = p.effect_class();
    }
    if (p.has_id())
    {
      res.id = p.id();
    }
    if (p.has_name())
    {
      res.name = p.name();
    }
    if (p.has_start_time())
    {
      res.startTime = p.start_time();
    }
    if (p.has_end_time())
    {
      res.endTime = p.end_time();
    }
    res.configMsg.resize(p.config_msg().size());
    memcpy(res.configMsg.data(), p.config_msg().data(), p.config_msg().size());
    if (p.has_config_file())
    {
      res.configFile = p.config_file();
    }
    return res;
  }

  inline void ToProtocol(const EffectSetting& v, effect::protocol::EffectSetting* p)
  {
    p->set_effect_class(v.effectClass);
    p->set_id(v.id);
    p->set_name(v.name);
    p->set_start_time(v.startTime);
    p->set_end_time(v.endTime);
    p->set_config_file(v.configFile);
  }

  inline EffectSettings FromProtocol(const effect::protocol::EffectSettings& p)
  {
    EffectSettings res;
    res.effectSetting = FromProtocolRepeated<EffectSetting>(p.effect_setting());
    if (p.has_soundtrack())
    {
      res.soundtrack = p.soundtrack();
    }
    return res;
  }

  inline void ToProtocol(const EffectSettings& v, effect::protocol::EffectSettings* p)
  {
    for (const auto& x : v.effectSetting)
      ToProtocol(x, p->add_effect_setting());
    p->set_soundtrack(v.soundtrack);
  }

	
}
