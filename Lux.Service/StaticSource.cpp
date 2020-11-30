#include "pch.h"
#include "StaticSource.h"
#include "SteadyTimer.h"

using namespace Lux::Configuration;
using namespace Lux::Graphics;
using namespace Lux::Events;
using namespace Lux::Threading;

using namespace std;
using namespace std::chrono;
using namespace std::chrono_literals;

namespace Lux::Sources
{
  StaticSource::StaticSource() :
    _worker(member_func(this, &StaticSource::Worker), L"static source")
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
    steady_timer timer{ 16ms };

    while (!_worker.is_shutting_down())
    {
      timer.wait();

      auto settings = Settings();
      if (!settings) continue;

      auto count = settings->SamplePoints.size();
      if (count == 0u) continue;

      EmitColors({ count, _color });
    }
  }
}