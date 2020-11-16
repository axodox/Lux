#include "pch.h"
#include "StaticSource.h"

using namespace Lux::Configuration;
using namespace Lux::Graphics;
using namespace Lux::Events;

using namespace std;
using namespace std::chrono;
using namespace std::chrono_literals;

namespace Lux::Sources
{
  StaticSource::StaticSource() :
    _lastRefresh(steady_clock::now()),
    _worker(member_func(this, &StaticSource::Worker))
  { }

  Graphics::rgb StaticSource::Color() const
  {
    return _color;
  }

  void StaticSource::Color(Graphics::rgb value)
  {
    _color = value;
  }

  Configuration::LightSourceKind StaticSource::Kind()
  {
    return LightSourceKind::Static;
  }

  void StaticSource::Worker()
  {
    const duration<float> refreshInterval = 16ms;

    while (!_worker.is_shutting_down())
    {
      auto currentRefresh = steady_clock::now();
      auto elapsedTime = currentRefresh - _lastRefresh;
      if (elapsedTime < refreshInterval)
      {
        this_thread::sleep_for(refreshInterval - elapsedTime);
      }

      auto settings = Settings();
      if (!settings) continue;

      auto count = settings->SamplePoints.size();
      if (count == 0u) continue;

      EmitColors({ count, _color });
    }
  }
}