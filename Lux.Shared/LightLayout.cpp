#include "pch.h"
#include "LightLayout.h"

namespace Lux::Configuration
{
  DisplaySize::DisplaySize(const Json::json_value& value)
  {
    Json::json_object object{ value };
    object.read(L"width", Width);
    object.read(L"height", Height);
  }
}