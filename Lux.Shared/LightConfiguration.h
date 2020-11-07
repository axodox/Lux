#pragma once
#include "ObservableObject.h"
#include "DeviceSettings.h"

namespace Lux::Configuration
{
  enum class LightConfigurationProperty : uint16_t
  {
    None = 0,
    Device = 1
  };

  struct LightConfiguration : public Observable::observable_object<LightConfigurationProperty>
  {
    Observable::observable_property<DeviceSettings> Device;

    LightConfiguration(const callback_t& callback);
  };
}