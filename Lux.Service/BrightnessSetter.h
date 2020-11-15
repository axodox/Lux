#pragma once
#include "ColorProcessor.h"

namespace Lux::Colors
{
  class BrightnessSetter : public ColorProcessor
  {
  public:
    uint8_t Brightness() const;
    void Brightness(uint8_t value);

    virtual void ProcessColors(std::vector<Graphics::rgb>& colors) override;

  private:
    uint8_t _brightness = 255ui8;
  };
}