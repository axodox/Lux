#pragma once
#include "Lights.DisplayLightConfiguration.g.h"

namespace winrt::Lux::Common::Lights::implementation
{
  struct DisplayLightConfiguration : DisplayLightConfigurationT<DisplayLightConfiguration>
  {
    DisplayLightConfiguration() = default;

    Windows::Foundation::Numerics::float2 DisplaySize();
    void DisplaySize(Windows::Foundation::Numerics::float2 const& value);
    com_array<Lux::Common::Lights::DisplayLightStripSegment> Segments();
    void Segments(array_view<Lux::Common::Lights::DisplayLightStripSegment const> value);

  private:
    Windows::Foundation::Numerics::float2 _displaySize;
    std::vector<Lux::Common::Lights::DisplayLightStripSegment> _segments;
  };
}
namespace winrt::Lux::Common::Lights::factory_implementation
{
  struct DisplayLightConfiguration : DisplayLightConfigurationT<DisplayLightConfiguration, implementation::DisplayLightConfiguration>
  {
  };
}
