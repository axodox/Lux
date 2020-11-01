#pragma once
#include "Json.h"

namespace Lux::Configuration
{
  struct DisplaySize
  {
    float Width, Height;

    DisplaySize() = default;
    DisplaySize(const Json::json_value& value);
  };
}