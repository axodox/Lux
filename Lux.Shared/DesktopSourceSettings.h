#pragma once
#include "ObservableObject.h"

namespace Lux::Configuration
{
  enum class DesktopSourceSettingsProperty : uint16_t
  {
    TemporalAveraging,
    SampleSize
  };

  struct DesktopSourceSettings : public Observable::observable_object<DesktopSourceSettingsProperty>
  {
    Observable::observable_property<float> TemporalAveraging;
    Observable::observable_property<float> SampleSize;

    DesktopSourceSettings(const callback_t& callback);
  };
}