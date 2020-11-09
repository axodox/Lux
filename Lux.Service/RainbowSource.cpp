#include "pch.h"
#include "RainbowSource.h"
#include "MathExtensions.h"

using namespace Lux::Graphics;
using namespace Lux::Math;

using namespace std;
using namespace std::chrono;
using namespace std::chrono_literals;

using namespace winrt::Windows::System::Threading;

namespace Lux::Sources
{
  RainbowSource::RainbowSource() :
    _spatialFrequency(1ui8),
    _angularVelocity(float(M_PI) / 10.f),
    _lastRefresh(steady_clock::now()),
    _lastAngle(0.f),
    _timer(ThreadPoolTimer::CreatePeriodicTimer({ this, &RainbowSource::OnTick }, 16ms))    
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

      auto angle = deg(wrap(atan2(position.y, position.x) + rotation, 0.f, float(M_PI)));
      colors.push_back(hsl{ angle, 1.f, 1.f });
    }

    EmitColors(move(colors));
  }
}