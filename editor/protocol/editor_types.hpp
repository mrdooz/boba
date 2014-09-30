#pragma once

#include "common_types.hpp"
#include "editor_types.hpp"

namespace editor { 
  struct Settings;
  struct StyleSetting;
  struct StyleSettings;
    
  struct Settings
  {

    u32 tickerHeight = 50;
    u32 tickerInterval = 20;
    u32 ticksPerInterval = 4;
    u32 effectViewWidth = 200;
    u32 effectRowHeight = 20;
    u32 statusBarHeight = 30;
    u32 effectHeight = 40;
    u32 resizeHandle = 6;
    u32 timelineZoomMin = 16;
    u32 timelineZoomMax = 1024;
    u32 timelineZoomDefault = 128;
    u32 keyframeSize = 5;
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
    u32 fontStyle;
  };
  struct StyleSettings
  {

    vector<StyleSetting> styleSetting;
  };
  
}