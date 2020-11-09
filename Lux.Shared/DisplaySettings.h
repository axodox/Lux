#pragma once
#include "DeviceSettings.h"

namespace Lux::Configuration
{
  struct DisplaySettings
  {
    float AspectRatio;
    std::vector<winrt::Windows::Foundation::Numerics::float2> SamplePoints;

    static DisplaySettings FromLayout(const DisplayLightLayout& layout);
  };
}