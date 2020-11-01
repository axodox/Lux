#pragma once
#include "ObservableObject.h"

namespace Lux::Configuration
{
  enum class LightConfigurationProperty : uint16_t
  {
    None = 0,
    IsEnabled = 1
  };

  struct LightConfiguration : public Observable::observable_object<LightConfigurationProperty>
  {
    Observable::observable_property<bool> IsEnabled;

    LightConfiguration(const callback_t& callback) : 
      Observable::observable_object<LightConfigurationProperty>(callback),
      IsEnabled(make_property(LightConfigurationProperty::IsEnabled, false))
    { }
  };
}