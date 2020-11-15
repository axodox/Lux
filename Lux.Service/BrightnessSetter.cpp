#include "pch.h"
#include "ColorProcessor.h"
#include "BrightnessSetter.h"

namespace Lux::Colors
{
  uint8_t BrightnessSetter::Brightness() const
  {
    return _brightness;
  }

  void BrightnessSetter::Brightness(uint8_t value)
  {
    _brightness = value;
  }

  void BrightnessSetter::ProcessColors(std::vector<Graphics::rgb>& colors)
  {
    if (_brightness == 255ui8) return;

    auto ratio = _brightness / 255.f;

    for (auto& color : colors)
    {
      color.r = uint8_t(ratio * color.r);
      color.g = uint8_t(ratio * color.g);
      color.b = uint8_t(ratio * color.b);
    }
  }
}