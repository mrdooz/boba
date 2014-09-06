#pragma once


namespace editor
{
  struct EffectSetting;
  struct EffectSettings;
    
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
    vector<uint8_t> configMsg;
    string configFile;
  };

  struct EffectSettings
  {
    vector<EffectSetting> effectSetting;
    string soundtrack;
  };

  
}