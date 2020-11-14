#include "pch.h"
#include "RainbowSourceSettings.h"

namespace Lux::Configuration
{
  RainbowSourceSettings::RainbowSourceSettings(const callback_t& callback) :
    Observable::observable_object<RainbowSourceSettingsProperty>(callback),
    SpatialFrequency(make_property(RainbowSourceSettingsProperty::SpatialFrequency, 2ui8)),
    AngularVelocity(make_property(RainbowSourceSettingsProperty::AngularVelocity, float(M_PI) / 5))
  { }
}