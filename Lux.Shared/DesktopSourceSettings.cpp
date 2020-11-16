#include "pch.h"
#include "DesktopSourceSettings.h"

namespace Lux::Configuration
{
  DesktopSourceSettings::DesktopSourceSettings(const callback_t& callback) :
    Observable::observable_object<DesktopSourceSettingsProperty>(callback),
    TemporalAveraging(make_property(DesktopSourceSettingsProperty::TemporalAveraging, 0.2f)),
    SampleSize(make_property(DesktopSourceSettingsProperty::SampleSize, 0.1f))
  { }
}