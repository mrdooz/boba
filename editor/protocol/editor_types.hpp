#pragma once

#include "common_types.hpp"
#include "editor_types.hpp"
#include "common_types.hpp"

namespace editor
{
  struct Settings;
  struct StyleSetting;
  struct StyleSettings;
    
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

  
}