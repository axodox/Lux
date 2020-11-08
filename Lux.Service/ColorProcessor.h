#pragma once
#include "Colors.h"

namespace Lux::Colors
{
  class ColorProcessor
  {
  public:
    virtual void ProcessColors(std::vector<Graphics::rgb>& colors) = 0;
    virtual ~ColorProcessor() = default;
  };
}