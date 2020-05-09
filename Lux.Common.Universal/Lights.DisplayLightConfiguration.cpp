#include "pch.h"
#include "Lights.DisplayLightConfiguration.h"
#include "Lights.DisplayLightConfiguration.g.cpp"

namespace winrt::Lux::Common::Lights::implementation
{
  Windows::Foundation::Numerics::float2 DisplayLightConfiguration::DisplaySize()
  {
    return _displaySize;
  }

  void DisplayLightConfiguration::DisplaySize(Windows::Foundation::Numerics::float2 const& value)
  {
    _displaySize = value;
  }

  com_array<Lux::Common::Lights::DisplayLightStripSegment> DisplayLightConfiguration::Segments()
  {
    return com_array<Lux::Common::Lights::DisplayLightStripSegment>{ _segments };
  }

  void DisplayLightConfiguration::Segments(array_view<Lux::Common::Lights::DisplayLightStripSegment const> value)
  {
    _segments = { value.begin(), value.end() };
  }
}
