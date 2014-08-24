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
  struct FloatKeyframe;
  struct FloatAnim;
  struct Vector3Keyframe;
  struct Vector3Anim;
  struct Settings;
  struct StyleSetting;
  struct StyleSettings;
  struct EffectSetting;
  struct EffectSettings;
  struct Plexus;
  struct TextPath;
  struct Displacement;
  struct NoiseEffector;

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
  EffectSetting FromProtocol(const effect::protocol::EffectSetting& p);
  void ToProtocol(const EffectSetting& v, effect::protocol::EffectSetting* p);
  EffectSettings FromProtocol(const effect::protocol::EffectSettings& p);
  void ToProtocol(const EffectSettings& v, effect::protocol::EffectSettings* p);
  Plexus FromProtocol(const effect::protocol::plexus::Plexus& p);
  void ToProtocol(const Plexus& v, effect::protocol::plexus::Plexus* p);
  TextPath FromProtocol(const effect::protocol::plexus::TextPath& p);
  void ToProtocol(const TextPath& v, effect::protocol::plexus::TextPath* p);
  Displacement FromProtocol(const effect::protocol::plexus::Displacement& p);
  void ToProtocol(const Displacement& v, effect::protocol::plexus::Displacement* p);
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
  
  struct FloatKeyframe
  {
    uint32_t time;
    float value;

    struct FlagsF {
      enum Enum { HasTime = 1 << 0, HasValue = 1 << 1, };
      struct Bits { u32 hasTime : 1; u32 hasValue : 1; };
    };
    Flags<FlagsF> flags;
  };

  struct FloatAnim
  {
    uint32_t type;
    vector<FloatKeyframe> keyframe;
    float minValue;
    float maxValue;
    float step;

    struct FlagsF {
      enum Enum { HasType = 1 << 0, HasKeyframe = 1 << 1, HasMinValue = 1 << 2, HasMaxValue = 1 << 3, HasStep = 1 << 4, };
      struct Bits { u32 hasType : 1; u32 hasKeyframe : 1; u32 hasMinValue : 1; u32 hasMaxValue : 1; u32 hasStep : 1; };
    };
    Flags<FlagsF> flags;
  };

  struct Vector3Keyframe
  {
    uint32_t time;
    Vector3f value;

    struct FlagsF {
      enum Enum { HasTime = 1 << 0, HasValue = 1 << 1, };
      struct Bits { u32 hasTime : 1; u32 hasValue : 1; };
    };
    Flags<FlagsF> flags;
  };

  struct Vector3Anim
  {
    uint32_t type;
    vector<Vector3Keyframe> keyframe;
    Vector3f minValue;
    Vector3f maxValue;
    Vector3f step;

    struct FlagsF {
      enum Enum { HasType = 1 << 0, HasKeyframe = 1 << 1, HasMinValue = 1 << 2, HasMaxValue = 1 << 3, HasStep = 1 << 4, };
      struct Bits { u32 hasType : 1; u32 hasKeyframe : 1; u32 hasMinValue : 1; u32 hasMaxValue : 1; u32 hasStep : 1; };
    };
    Flags<FlagsF> flags;
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
    uint32_t keyframeSize;
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
    Color varTextColor;
    Color varValueColor;
    Color effectIconExpandedColor;
    Color effectIconCollapsedColor;

    struct FlagsF {
      enum Enum { HasTickerHeight = 1 << 0, HasTickerInterval = 1 << 1, HasTicksPerInterval = 1 << 2, HasEffectViewWidth = 1 << 3, HasEffectRowHeight = 1 << 4, HasStatusBarHeight = 1 << 5, HasEffectHeight = 1 << 6, HasResizeHandle = 1 << 7, HasTimelineZoomMin = 1 << 8, HasTimelineZoomMax = 1 << 9, HasTimelineZoomDefault = 1 << 10, HasKeyframeSize = 1 << 11, HasDefaultRowColor = 1 << 12, HasSelectedRowColor = 1 << 13, HasHoverRowColor = 1 << 14, HasInvalidHoverRowColor = 1 << 15, HasDefaultLineColor = 1 << 16, HasEffectViewBackgroundColor = 1 << 17, HasEffectViewBackgroundColorSelected = 1 << 18, HasTimelineViewBackgroundColor = 1 << 19, HasEffectLineColor = 1 << 20, HasKeyframeDefaultColor = 1 << 21, HasKeyframeSelectedColor = 1 << 22, HasVarTextColor = 1 << 23, HasVarValueColor = 1 << 24, HasEffectIconExpandedColor = 1 << 25, HasEffectIconCollapsedColor = 1 << 26, };
      struct Bits { u32 hasTickerHeight : 1; u32 hasTickerInterval : 1; u32 hasTicksPerInterval : 1; u32 hasEffectViewWidth : 1; u32 hasEffectRowHeight : 1; u32 hasStatusBarHeight : 1; u32 hasEffectHeight : 1; u32 hasResizeHandle : 1; u32 hasTimelineZoomMin : 1; u32 hasTimelineZoomMax : 1; u32 hasTimelineZoomDefault : 1; u32 hasKeyframeSize : 1; u32 hasDefaultRowColor : 1; u32 hasSelectedRowColor : 1; u32 hasHoverRowColor : 1; u32 hasInvalidHoverRowColor : 1; u32 hasDefaultLineColor : 1; u32 hasEffectViewBackgroundColor : 1; u32 hasEffectViewBackgroundColorSelected : 1; u32 hasTimelineViewBackgroundColor : 1; u32 hasEffectLineColor : 1; u32 hasKeyframeDefaultColor : 1; u32 hasKeyframeSelectedColor : 1; u32 hasVarTextColor : 1; u32 hasVarValueColor : 1; u32 hasEffectIconExpandedColor : 1; u32 hasEffectIconCollapsedColor : 1; };
    };
    Flags<FlagsF> flags;
  };

  struct StyleSetting
  {
    string id;
    Color fillColor;
    Color outlineColor;
    float outlineThickness;
    uint32_t fontStyle;

    struct FlagsF {
      enum Enum { HasId = 1 << 0, HasFillColor = 1 << 1, HasOutlineColor = 1 << 2, HasOutlineThickness = 1 << 3, HasFontStyle = 1 << 4, };
      struct Bits { u32 hasId : 1; u32 hasFillColor : 1; u32 hasOutlineColor : 1; u32 hasOutlineThickness : 1; u32 hasFontStyle : 1; };
    };
    Flags<FlagsF> flags;
  };

  struct StyleSettings
  {
    vector<StyleSetting> styleSetting;

    struct FlagsF {
      enum Enum { HasStyleSetting = 1 << 0, };
      struct Bits { u32 hasStyleSetting : 1; };
    };
    Flags<FlagsF> flags;
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

    struct FlagsF {
      enum Enum { HasType = 1 << 0, HasEffectClass = 1 << 1, HasId = 1 << 2, HasName = 1 << 3, HasStartTime = 1 << 4, HasEndTime = 1 << 5, HasConfigMsg = 1 << 6, HasConfigFile = 1 << 7, };
      struct Bits { u32 hasType : 1; u32 hasEffectClass : 1; u32 hasId : 1; u32 hasName : 1; u32 hasStartTime : 1; u32 hasEndTime : 1; u32 hasConfigMsg : 1; u32 hasConfigFile : 1; };
    };
    Flags<FlagsF> flags;
  };

  struct EffectSettings
  {
    vector<EffectSetting> effectSetting;
    string soundtrack;

    struct FlagsF {
      enum Enum { HasEffectSetting = 1 << 0, HasSoundtrack = 1 << 1, };
      struct Bits { u32 hasEffectSetting : 1; u32 hasSoundtrack : 1; };
    };
    Flags<FlagsF> flags;
  };

  struct Plexus
  {
    vector<TextPath> textPaths;
    vector<NoiseEffector> noiseEffectors;

    struct FlagsF {
      enum Enum { HasTextPaths = 1 << 0, HasNoiseEffectors = 1 << 1, };
      struct Bits { u32 hasTextPaths : 1; u32 hasNoiseEffectors : 1; };
    };
    Flags<FlagsF> flags;
  };

  struct TextPath
  {
    string text;

    struct FlagsF {
      enum Enum { HasText = 1 << 0, };
      struct Bits { u32 hasText : 1; };
    };
    Flags<FlagsF> flags;
  };

  struct Displacement
  {
    FloatAnim x;
    FloatAnim y;
    FloatAnim z;

    struct FlagsF {
      enum Enum { HasX = 1 << 0, HasY = 1 << 1, HasZ = 1 << 2, };
      struct Bits { u32 hasX : 1; u32 hasY : 1; u32 hasZ : 1; };
    };
    Flags<FlagsF> flags;
  };

  struct NoiseEffector
  {
    enum class ApplyTo
    {
      Position = 1,
      Scale = 2,
    };

    NoiseEffector::ApplyTo applyTo;
    Displacement displacement;

    struct FlagsF {
      enum Enum { HasApplyTo = 1 << 0, HasDisplacement = 1 << 1, };
      struct Bits { u32 hasApplyTo : 1; u32 hasDisplacement : 1; };
    };
    Flags<FlagsF> flags;
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

  inline FloatKeyframe FromProtocol(const common::protocol::FloatKeyframe& p)
  {
    FloatKeyframe res;
    if (p.has_time())
    {
      res.flags.Set(FloatKeyframe::FlagsF::HasTime);
      res.time = p.time();
    }
    if (p.has_value())
    {
      res.flags.Set(FloatKeyframe::FlagsF::HasValue);
      res.value = p.value();
    }
    return res;
  }

  inline void ToProtocol(const FloatKeyframe& v, common::protocol::FloatKeyframe* p)
  {
    p->set_time(v.time);
    p->set_value(v.value);
  }

  inline FloatAnim FromProtocol(const common::protocol::FloatAnim& p)
  {
    FloatAnim res;
    if (p.has_type())
    {
      res.flags.Set(FloatAnim::FlagsF::HasType);
      res.type = p.type();
    }
    res.keyframe = FromProtocolRepeated<FloatKeyframe>(p.keyframe());
    if (p.has_min_value())
    {
      res.flags.Set(FloatAnim::FlagsF::HasMinValue);
      res.minValue = p.min_value();
    }
    if (p.has_max_value())
    {
      res.flags.Set(FloatAnim::FlagsF::HasMaxValue);
      res.maxValue = p.max_value();
    }
    if (p.has_step())
    {
      res.flags.Set(FloatAnim::FlagsF::HasStep);
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
      res.flags.Set(Vector3Keyframe::FlagsF::HasTime);
      res.time = p.time();
    }
    if (p.has_value())
    {
      res.flags.Set(Vector3Keyframe::FlagsF::HasValue);
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
      res.flags.Set(Vector3Anim::FlagsF::HasType);
      res.type = p.type();
    }
    res.keyframe = FromProtocolRepeated<Vector3Keyframe>(p.keyframe());
    if (p.has_min_value())
    {
      res.flags.Set(Vector3Anim::FlagsF::HasMinValue);
      res.minValue = FromProtocol(p.min_value());
    }
    if (p.has_max_value())
    {
      res.flags.Set(Vector3Anim::FlagsF::HasMaxValue);
      res.maxValue = FromProtocol(p.max_value());
    }
    if (p.has_step())
    {
      res.flags.Set(Vector3Anim::FlagsF::HasStep);
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
      res.flags.Set(Settings::FlagsF::HasTickerHeight);
      res.tickerHeight = p.ticker_height();
    }
    if (p.has_ticker_interval())
    {
      res.flags.Set(Settings::FlagsF::HasTickerInterval);
      res.tickerInterval = p.ticker_interval();
    }
    if (p.has_ticks_per_interval())
    {
      res.flags.Set(Settings::FlagsF::HasTicksPerInterval);
      res.ticksPerInterval = p.ticks_per_interval();
    }
    if (p.has_effect_view_width())
    {
      res.flags.Set(Settings::FlagsF::HasEffectViewWidth);
      res.effectViewWidth = p.effect_view_width();
    }
    if (p.has_effect_row_height())
    {
      res.flags.Set(Settings::FlagsF::HasEffectRowHeight);
      res.effectRowHeight = p.effect_row_height();
    }
    if (p.has_status_bar_height())
    {
      res.flags.Set(Settings::FlagsF::HasStatusBarHeight);
      res.statusBarHeight = p.status_bar_height();
    }
    if (p.has_effect_height())
    {
      res.flags.Set(Settings::FlagsF::HasEffectHeight);
      res.effectHeight = p.effect_height();
    }
    if (p.has_resize_handle())
    {
      res.flags.Set(Settings::FlagsF::HasResizeHandle);
      res.resizeHandle = p.resize_handle();
    }
    if (p.has_timeline_zoom_min())
    {
      res.flags.Set(Settings::FlagsF::HasTimelineZoomMin);
      res.timelineZoomMin = p.timeline_zoom_min();
    }
    if (p.has_timeline_zoom_max())
    {
      res.flags.Set(Settings::FlagsF::HasTimelineZoomMax);
      res.timelineZoomMax = p.timeline_zoom_max();
    }
    if (p.has_timeline_zoom_default())
    {
      res.flags.Set(Settings::FlagsF::HasTimelineZoomDefault);
      res.timelineZoomDefault = p.timeline_zoom_default();
    }
    if (p.has_keyframe_size())
    {
      res.flags.Set(Settings::FlagsF::HasKeyframeSize);
      res.keyframeSize = p.keyframe_size();
    }
    if (p.has_default_row_color())
    {
      res.flags.Set(Settings::FlagsF::HasDefaultRowColor);
      res.defaultRowColor = FromProtocol(p.default_row_color());
    }
    if (p.has_selected_row_color())
    {
      res.flags.Set(Settings::FlagsF::HasSelectedRowColor);
      res.selectedRowColor = FromProtocol(p.selected_row_color());
    }
    if (p.has_hover_row_color())
    {
      res.flags.Set(Settings::FlagsF::HasHoverRowColor);
      res.hoverRowColor = FromProtocol(p.hover_row_color());
    }
    if (p.has_invalid_hover_row_color())
    {
      res.flags.Set(Settings::FlagsF::HasInvalidHoverRowColor);
      res.invalidHoverRowColor = FromProtocol(p.invalid_hover_row_color());
    }
    if (p.has_default_line_color())
    {
      res.flags.Set(Settings::FlagsF::HasDefaultLineColor);
      res.defaultLineColor = FromProtocol(p.default_line_color());
    }
    if (p.has_effect_view_background_color())
    {
      res.flags.Set(Settings::FlagsF::HasEffectViewBackgroundColor);
      res.effectViewBackgroundColor = FromProtocol(p.effect_view_background_color());
    }
    if (p.has_effect_view_background_color_selected())
    {
      res.flags.Set(Settings::FlagsF::HasEffectViewBackgroundColorSelected);
      res.effectViewBackgroundColorSelected = FromProtocol(p.effect_view_background_color_selected());
    }
    if (p.has_timeline_view_background_color())
    {
      res.flags.Set(Settings::FlagsF::HasTimelineViewBackgroundColor);
      res.timelineViewBackgroundColor = FromProtocol(p.timeline_view_background_color());
    }
    if (p.has_effect_line_color())
    {
      res.flags.Set(Settings::FlagsF::HasEffectLineColor);
      res.effectLineColor = FromProtocol(p.effect_line_color());
    }
    if (p.has_keyframe_default_color())
    {
      res.flags.Set(Settings::FlagsF::HasKeyframeDefaultColor);
      res.keyframeDefaultColor = FromProtocol(p.keyframe_default_color());
    }
    if (p.has_keyframe_selected_color())
    {
      res.flags.Set(Settings::FlagsF::HasKeyframeSelectedColor);
      res.keyframeSelectedColor = FromProtocol(p.keyframe_selected_color());
    }
    if (p.has_var_text_color())
    {
      res.flags.Set(Settings::FlagsF::HasVarTextColor);
      res.varTextColor = FromProtocol(p.var_text_color());
    }
    if (p.has_var_value_color())
    {
      res.flags.Set(Settings::FlagsF::HasVarValueColor);
      res.varValueColor = FromProtocol(p.var_value_color());
    }
    if (p.has_effect_icon_expanded_color())
    {
      res.flags.Set(Settings::FlagsF::HasEffectIconExpandedColor);
      res.effectIconExpandedColor = FromProtocol(p.effect_icon_expanded_color());
    }
    if (p.has_effect_icon_collapsed_color())
    {
      res.flags.Set(Settings::FlagsF::HasEffectIconCollapsedColor);
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
      res.flags.Set(StyleSetting::FlagsF::HasId);
      res.id = p.id();
    }
    if (p.has_fill_color())
    {
      res.flags.Set(StyleSetting::FlagsF::HasFillColor);
      res.fillColor = FromProtocol(p.fill_color());
    }
    if (p.has_outline_color())
    {
      res.flags.Set(StyleSetting::FlagsF::HasOutlineColor);
      res.outlineColor = FromProtocol(p.outline_color());
    }
    if (p.has_outline_thickness())
    {
      res.flags.Set(StyleSetting::FlagsF::HasOutlineThickness);
      res.outlineThickness = p.outline_thickness();
    }
    if (p.has_font_style())
    {
      res.flags.Set(StyleSetting::FlagsF::HasFontStyle);
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

  inline EffectSetting FromProtocol(const effect::protocol::EffectSetting& p)
  {
    EffectSetting res;
    res.type = (EffectSetting::Type)p.type();
    if (p.has_effect_class())
    {
      res.flags.Set(EffectSetting::FlagsF::HasEffectClass);
      res.effectClass = p.effect_class();
    }
    if (p.has_id())
    {
      res.flags.Set(EffectSetting::FlagsF::HasId);
      res.id = p.id();
    }
    if (p.has_name())
    {
      res.flags.Set(EffectSetting::FlagsF::HasName);
      res.name = p.name();
    }
    if (p.has_start_time())
    {
      res.flags.Set(EffectSetting::FlagsF::HasStartTime);
      res.startTime = p.start_time();
    }
    if (p.has_end_time())
    {
      res.flags.Set(EffectSetting::FlagsF::HasEndTime);
      res.endTime = p.end_time();
    }
    res.configMsg.resize(p.config_msg().size());
    memcpy(res.configMsg.data(), p.config_msg().data(), p.config_msg().size());
    if (p.has_config_file())
    {
      res.flags.Set(EffectSetting::FlagsF::HasConfigFile);
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
      res.flags.Set(EffectSettings::FlagsF::HasSoundtrack);
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
      res.flags.Set(TextPath::FlagsF::HasText);
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
      res.flags.Set(Displacement::FlagsF::HasX);
      res.x = FromProtocol(p.x());
    }
    if (p.has_y())
    {
      res.flags.Set(Displacement::FlagsF::HasY);
      res.y = FromProtocol(p.y());
    }
    if (p.has_z())
    {
      res.flags.Set(Displacement::FlagsF::HasZ);
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
      res.flags.Set(NoiseEffector::FlagsF::HasDisplacement);
      res.displacement = FromProtocol(p.displacement());
    }
    return res;
  }

  inline void ToProtocol(const NoiseEffector& v, effect::protocol::plexus::NoiseEffector* p)
  {
    ToProtocol(v.displacement, p->mutable_displacement());
  }

	
}
