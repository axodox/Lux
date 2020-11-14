#pragma once
#include "ObservableObject.h"
#include "Colors.h"

namespace Lux::Configuration
{
  enum class RainbowSourceSettingsProperty : uint16_t
  {
    SpatialFrequency,
    AngularVelocity
  };

  struct RainbowSourceSettings : public Observable::observable_object<RainbowSourceSettingsProperty>
  {
    Observable::observable_property<uint8_t> SpatialFrequency;
    Observable::observable_property<float> AngularVelocity;

    RainbowSourceSettings(const callback_t& callback);
  };
}