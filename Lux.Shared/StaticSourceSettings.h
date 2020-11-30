#pragma once
#include "ObservableObject.h"
#include "Colors.h"

namespace Lux::Configuration
{
  enum class StaticSourceSettingsProperty : uint16_t
  {
    Color
  };

  struct StaticSourceSettings : public Observable::observable_object<StaticSourceSettingsProperty>
  {
    Observable::observable_property<Graphics::rgb> Color;

    StaticSourceSettings(const callback_t& callback);
  };
}