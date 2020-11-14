#pragma once
#include "ObservableObject.h"
#include "DeviceSettings.h"
#include "StaticSourceSettings.h"
#include "RainbowSourceSettings.h"
#include "Colors.h"

namespace Lux::Configuration
{
  enum class LightSourceKind : uint8_t
  {
    Off,
    Static,
    Rainbow,
    ContextAware
  };

  enum class LightConfigurationProperty : uint16_t
  {
    None,
    Device,
    LightSource,
    StaticSourceOptions,
    RainbowSourceOptions
  };

  struct LightConfiguration : public Observable::observable_object<LightConfigurationProperty>
  {
    Observable::observable_property<DeviceSettings> Device;
    Observable::observable_property<LightSourceKind> LightSource;

    Observable::observable_property<StaticSourceSettings> StaticSourceOptions;
    Observable::observable_property<RainbowSourceSettings> RainbowSourceOptions;

    LightConfiguration(const callback_t& callback);
  };
}