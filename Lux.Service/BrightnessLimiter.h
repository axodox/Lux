#pragma once
#include "ColorProcessor.h"

namespace Lux::Colors
{
  class BrightnessLimiter : public ColorProcessor
  {
  public:
    uint8_t MaxBrightness() const;
    void MaxBrightness(uint8_t value);

    virtual void ProcessColors(std::vector<Graphics::rgb>& colors) override;

  private:
    uint8_t _max = 178;
  };
}