#include "pch.h"
#include "StaticSourceSettings.h"

namespace Lux::Configuration
{
  StaticSourceSettings::StaticSourceSettings(const callback_t& callback) :
    Observable::observable_object<StaticSourceSettingsProperty>(callback),
    Color(make_property(StaticSourceSettingsProperty::Color, Graphics::rgb{ 255ui8, 255ui8 , 255ui8 }))
  { }
}