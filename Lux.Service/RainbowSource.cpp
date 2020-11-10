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

    auto now = steady_clock::now();
    auto elapsedTime = duration_cast<duration<float>>(now - _lastRefresh);

    auto rotation = _lastAngle + elapsedTime.count() * _angularVelocity;

    vector<rgb> colors;
    colors.reserve(count);

    for (auto& light : settings->SamplePoints)
    {
      auto position = light;
      position.x *= settings->AspectRatio;

      auto angle = deg(wrap(_spatialFrequency * atan2(position.y - 0.5f, position.x - 0.5f) + rotation, 0.f, 2 * float(M_PI)));
      colors.push_back(hsl{ angle, 1.f, 0.5f });
    }

    EmitColors(move(colors));
  }
}