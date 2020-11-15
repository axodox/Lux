#include "pch.h"
#include "LightSource.h"

using namespace Lux::Configuration;
using namespace Lux::Graphics;
using namespace Lux::Threading;

using namespace std;

namespace Lux::Sources
{
  LightSource::LightSource() :
    ColorsEmitted(_events)
  { }

  Threading::locked_ref<Configuration::DisplaySettings> LightSource::Settings()
  {
    return _settings.get();
  }

  void LightSource::Settings(DisplaySettings&& value)
  {
    _settings = make_unique<DisplaySettings>(move(value));
    OnSettingsChanged();
  }

  void LightSource::EmitColors(vector<rgb>&& colors)
  {
    _events.raise(ColorsEmitted, this, move(colors));
  }
}
