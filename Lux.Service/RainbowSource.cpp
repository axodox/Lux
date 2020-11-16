#include "pch.h"
#include "RainbowSource.h"
#include "MathExtensions.h"

using namespace Lux::Configuration;
using namespace Lux::Graphics;
using namespace Lux::Events;
using namespace Lux::Math;

using namespace std;
using namespace std::chrono;
using namespace std::chrono_literals;

namespace Lux::Sources
{
  RainbowSource::RainbowSource() :
    _spatialFrequency(3ui8),
    _angularVelocity(float(M_PI) / 0.5f),
    _lastRefresh(steady_clock::now()),
    _lastAngle(0.f),
    _worker(member_func(this, &RainbowSource::Worker))
  { }

  uint8_t RainbowSource::SpatialFrequency() const
  {
    return _spatialFrequency;
  }

  void RainbowSource::SpatialFrequency(uint8_t value)
  {
    _spatialFrequency = value;
  }

  float RainbowSource::AngularVelocity() const
  {
    return _angularVelocity;
  }

  void RainbowSource::AngularVelocity(float value)
  {
    _angularVelocity = value;
  }

  Configuration::LightSourceKind RainbowSource::Kind()
  {
    return LightSourceKind::Rainbow;
  }

  void RainbowSource::Worker()
  {
    const duration<float> refreshInterval = 16ms;

    while (!_worker.is_shutting_down())
    {
      auto currentRefresh = steady_clock::now();
      auto elapsedTime = duration_cast<duration<float>>(currentRefresh - _lastRefresh);
      if (elapsedTime < refreshInterval)
      {
        this_thread::sleep_for(refreshInterval - elapsedTime);
      }

      auto settings = Settings();
      if (!settings) continue;

      auto count = settings->SamplePoints.size();
      if (count == 0u) continue;

      auto currentAngle = _lastAngle + refreshInterval.count() * _angularVelocity;

      _lastRefresh = currentRefresh;
      _lastAngle = currentAngle;

      vector<rgb> colors;
      colors.reserve(count);

      for (auto& light : settings->SamplePoints)
      {
        auto rotation = deg(wrap(_spatialFrequency * (atan2(light.y - 0.5f, settings->AspectRatio * (light.x - 0.5f)) + float(M_PI_2)) + max(1ui8, _spatialFrequency) * currentAngle, 0.f, 2 * float(M_PI)));
        colors.push_back(hsl{ rotation, 1.f, 0.5f });
      }

      EmitColors(move(colors));
    }
  }
}