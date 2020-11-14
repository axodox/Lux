#include "pch.h"
#include "LightConfiguration.h"

using namespace winrt::Windows::Foundation::Numerics;

namespace Lux::Configuration
{
  LightConfiguration::LightConfiguration(const callback_t& callback) :
    Observable::observable_object<LightConfigurationProperty>(callback),
    Device(make_property(LightConfigurationProperty::Device, DeviceSettings{})),
    LightSource(make_property(LightConfigurationProperty::LightSource, LightSourceKind{})),

    StaticSourceOptions(make_property<StaticSourceSettings>(LightConfigurationProperty::StaticSourceOptions)),
    RainbowSourceOptions(make_property<RainbowSourceSettings>(LightConfigurationProperty::RainbowSourceOptions)),

    Brightness(make_property(LightConfigurationProperty::Brightness, 255ui8)),
    BrightnessLimit(make_property(LightConfigurationProperty::BrightnessLimit, 168ui8)),
    Gamma(make_property(LightConfigurationProperty::Gamma, float3(1.6f, 1.5f, 1.6f)))
  { }
}