#include "pch.h"
#include "ColorProcessor.h"
#include "BrightnessLimiter.h"

namespace Lux::Colors
{
  uint8_t BrightnessLimiter::MaxBrightness() const
  {
    return _max;
  }

  void BrightnessLimiter::MaxBrightness(uint8_t value)
  {
    _max = value;
  }

  void BrightnessLimiter::ProcessColors(std::vector<Graphics::rgb>& colors)
  {
    if (_max == 255ui8) return;

    //Sum colors
    uint32_t sum = 0u;
    for (auto& color : colors)
    {
      sum += color.r + color.g + color.b;
    }

    //Calculate average brightness
    auto avg = sum / 255.f / 3.f / colors.size();

    //Scale brightness is above limit
    if (avg > _max)
    {
      auto factor = _max / avg;
      for (auto& rgb : colors)
      {
        rgb.r = uint8_t(rgb.r * factor);
        rgb.g = uint8_t(rgb.g * factor);
        rgb.b = uint8_t(rgb.b * factor);
      }
    }
  }
}