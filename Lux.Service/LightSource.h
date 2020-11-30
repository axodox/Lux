#pragma once
#include "Events.h"
#include "Colors.h"
#include "DisplaySettings.h"
#include "LockedPtr.h"
#include "LightConfiguration.h"

namespace Lux::Sources
{
  class LightSource
  {
  private:
    Events::event_owner _events;
    Threading::locked_ptr<Configuration::DisplaySettings> _settings;

  protected:
    void EmitColors(std::vector<Graphics::rgb>&& colors);

    virtual void OnSettingsChanged() { }

  public:
    Events::event_publisher<LightSource*, std::vector<Graphics::rgb>&&> ColorsEmitted;

    LightSource();
    virtual ~LightSource() = default;

    virtual Configuration::LightSourceKind Kind() = 0;

    Threading::locked_ref<Configuration::DisplaySettings> Settings();
    void Settings(Configuration::DisplaySettings&& value);
  };
}