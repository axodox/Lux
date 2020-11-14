#include "pch.h"
#include "RainbowSource.h"
#include "MathExtensions.h"

using namespace Lux::Configuration;
using namespace Lux::Graphics;
using namespace Lux::Math;

using namespace std;
using namespace std::chrono;
using namespace std::chrono_literals;

using namespace winrt::Windows::System::Threading;

namespace Lux::Sources
{
  RainbowSource::RainbowSource() :
    _spatialFrequency(3ui8),
    _angularVelocity(float(M_PI) / 0.5f),
    _lastRefresh(steady_clock::now()),
    _lastAngle(0.f),
    _timer(ThreadPoolTimer::CreatePeriodicTimer({ this, &RainbowSource::OnTick }, 16ms))    
  { }

  RainbowSource::~RainbowSource()
  {
    _timer.Cancel();
  }

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

  void RainbowSource::OnTick(const ThreadPoolTimer& /*timer*/)
  {
    auto settings = Settings();
    if (!settings) return;

    auto count = settings->SamplePoints.size();
    if (count == 0u) return;

    auto currentRefresh = steady_clock::now();
    auto elapsedTime = duration_cast<duration<float>>(currentRefresh - _lastRefresh);
    auto currentAngle = _lastAngle + elapsedTime.count() * _angularVelocity;

    _lastRefresh = currentRefresh;
    _lastAngle = currentAngle;

    vector<rgb> colors;
    colors.reserve(count);

    for (auto& light : settings->SamplePoints)
    {
      auto rotation = deg(wrap(_spatialFrequency * (atan2(light.y - 0.5f, settings->AspectRatio * (light.x - 0.5f)) + float(M_PI_2)) + currentAngle, 0.f, 2 * float(M_PI)));
      colors.push_back(hsl{ rotation, 1.f, 0.5f });
    }

    EmitColors(move(colors));
  }
}