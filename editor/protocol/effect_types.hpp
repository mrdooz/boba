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
    u32 id;
    string name;
    u32 startTime;
    u32 endTime;
    vector<uint8_t> configMsg;
    string configFile;
  };

  struct EffectSettings
  {

    vector<EffectSetting> effectSetting;
    string soundtrack;
  };

  
}