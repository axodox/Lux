#include "pch.h"
#include "LightConfiguration.h"

namespace Lux::Configuration
{
  LightConfiguration::LightConfiguration(const callback_t& callback) :
    Observable::observable_object<LightConfigurationProperty>(callback),
    Device(make_property(LightConfigurationProperty::Device, DeviceSettings{})),
    LightSource(make_property(LightConfigurationProperty::LightSource, LightSourceKind{}))
  { }
}