#pragma once
#include "ObservableObject.h"
#include "DeviceSettings.h"

namespace Lux::Configuration
{
  enum class LightConfigurationProperty : uint16_t
  {
    None,
    Device,
    LightSource
  };

  enum class LightSourceKind : uint8_t
  {
    Off,
    Static,
    Rainbow,
    ContextAware
  };

  struct LightConfiguration : public Observable::observable_object<LightConfigurationProperty>
  {
    Observable::observable_property<DeviceSettings> Device;
    Observable::observable_property<LightSourceKind> LightSource;

    LightConfiguration(const callback_t& callback);
  };
}