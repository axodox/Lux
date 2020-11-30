#pragma once
#include "ObservableObject.h"
#include "DeviceSettings.h"
#include "StaticSourceSettings.h"
#include "RainbowSourceSettings.h"
#include "DesktopSourceSettings.h"
#include "Colors.h"

namespace Lux::Configuration
{
  enum class LightSourceKind : uint8_t
  {
    Off,
    Static,
    Rainbow,
    Desktop
  };

  enum class LightConfigurationProperty : uint16_t
  {
    None,
    Device,
    IsConnected,

    LightSource,
    StaticSourceOptions,
    RainbowSourceOptions,
    DesktopSourceOptions,

    Saturation,
    Brightness,
    BrightnessLimit,
    Gamma
  };

  struct LightConfiguration : public Observable::observable_object<LightConfigurationProperty>
  {
    Observable::observable_property<DeviceSettings> Device;
    Observable::observable_property<bool> IsConnected;

    Observable::observable_property<LightSourceKind> LightSource;
    Observable::observable_property<StaticSourceSettings> StaticSourceOptions;
    Observable::observable_property<RainbowSourceSettings> RainbowSourceOptions;
    Observable::observable_property<DesktopSourceSettings> DesktopSourceOptions;

    Observable::observable_property<float> Saturation;
    Observable::observable_property<uint8_t> Brightness;
    Observable::observable_property<uint8_t> BrightnessLimit;
    Observable::observable_property<winrt::Windows::Foundation::Numerics::float3> Gamma;

    LightConfiguration(const callback_t& callback);
  };
}