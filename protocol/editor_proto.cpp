#include "editor_proto.hpp"

namespace boba
{
  Settings FromProtocol(const protocol::editor::Settings& p)
  {
    Settings res;
    if (p.has_ticker_height())
      res.tickerHeight = p.ticker_height();

    if (p.has_ticker_interval())
      res.tickerInterval = p.ticker_interval();

    if (p.has_ticks_per_interval())
      res.ticksPerInterval = p.ticks_per_interval();

    if (p.has_effect_view_width())
      res.effectViewWidth = p.effect_view_width();

    if (p.has_effect_row_height())
      res.effectRowHeight = p.effect_row_height();

    if (p.has_status_bar_height())
      res.statusBarHeight = p.status_bar_height();

    if (p.has_effect_height())
      res.effectHeight = p.effect_height();

    if (p.has_resize_handle())
      res.resizeHandle = p.resize_handle();

    if (p.has_timeline_zoom_min())
      res.timelineZoomMin = p.timeline_zoom_min();

    if (p.has_timeline_zoom_max())
      res.timelineZoomMax = p.timeline_zoom_max();

    if (p.has_timeline_zoom_default())
      res.timelineZoomDefault = p.timeline_zoom_default();

    if (p.has_keyframe_size())
      res.keyframeSize = p.keyframe_size();

    if (p.has_default_row_color())
      res.defaultRowColor = FromProtocol(p.default_row_color());

    if (p.has_selected_row_color())
      res.selectedRowColor = FromProtocol(p.selected_row_color());

    if (p.has_hover_row_color())
      res.hoverRowColor = FromProtocol(p.hover_row_color());

    if (p.has_invalid_hover_row_color())
      res.invalidHoverRowColor = FromProtocol(p.invalid_hover_row_color());

    if (p.has_default_line_color())
      res.defaultLineColor = FromProtocol(p.default_line_color());

    if (p.has_effect_view_background_color())
      res.effectViewBackgroundColor = FromProtocol(p.effect_view_background_color());

    if (p.has_effect_view_background_color_selected())
      res.effectViewBackgroundColorSelected = FromProtocol(p.effect_view_background_color_selected());

    if (p.has_timeline_view_background_color())
      res.timelineViewBackgroundColor = FromProtocol(p.timeline_view_background_color());

    if (p.has_effect_line_color())
      res.effectLineColor = FromProtocol(p.effect_line_color());

    if (p.has_keyframe_default_color())
      res.keyframeDefaultColor = FromProtocol(p.keyframe_default_color());

    if (p.has_keyframe_selected_color())
      res.keyframeSelectedColor = FromProtocol(p.keyframe_selected_color());

    if (p.has_keyframe_control_color())
      res.keyframeControlColor = FromProtocol(p.keyframe_control_color());

    if (p.has_graph_color())
      res.graphColor = FromProtocol(p.graph_color());

    if (p.has_var_text_color())
      res.varTextColor = FromProtocol(p.var_text_color());

    if (p.has_var_value_color())
      res.varValueColor = FromProtocol(p.var_value_color());

    if (p.has_effect_icon_expanded_color())
      res.effectIconExpandedColor = FromProtocol(p.effect_icon_expanded_color());

    if (p.has_effect_icon_collapsed_color())
      res.effectIconCollapsedColor = FromProtocol(p.effect_icon_collapsed_color());

    return res;
  }

  void ToProtocol(const Settings& v, protocol::editor::Settings* p)
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

  StyleSetting FromProtocol(const protocol::editor::StyleSetting& p)
  {
    StyleSetting res;
    if (p.has_id())
      res.id = p.id();

    if (p.has_fill_color())
      res.fillColor = FromProtocol(p.fill_color());

    if (p.has_outline_color())
      res.outlineColor = FromProtocol(p.outline_color());

    if (p.has_outline_thickness())
      res.outlineThickness = p.outline_thickness();

    if (p.has_font_style())
      res.fontStyle = p.font_style();

    return res;
  }

  void ToProtocol(const StyleSetting& v, protocol::editor::StyleSetting* p)
  {
    p->set_id(v.id);
    ToProtocol(v.fillColor, p->mutable_fill_color());
    ToProtocol(v.outlineColor, p->mutable_outline_color());
    p->set_outline_thickness(v.outlineThickness);
    p->set_font_style(v.fontStyle);
  }

  StyleSettings FromProtocol(const protocol::editor::StyleSettings& p)
  {
    StyleSettings res;
    for (const auto& x : p.style_setting() )
      res.styleSetting.push_back(FromProtocol(x));

    return res;
  }

  void ToProtocol(const StyleSettings& v, protocol::editor::StyleSettings* p)
  {
    for (const auto& x : v.styleSetting)
      ToProtocol(x, p->add_style_setting());
  }

	
}
