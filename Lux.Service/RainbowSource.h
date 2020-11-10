#pragma once
#include "pch.h"
#include "LightSource.h"

namespace Lux::Sources
{
  class RainbowSource : public LightSource
  {
  public:
    RainbowSource();
    virtual ~RainbowSource();

    uint8_t SpatialFrequency() const;
    void SpatialFrequency(uint8_t value);

    float AngularVelocity() const;
    void AngularVelocity(float value);

    virtual Configuration::LightSourceKind Kind() override;

  private:
    uint8_t _spatialFrequency;
    float _angularVelocity;
    std::chrono::steady_clock::time_point _lastRefresh;
    float _lastAngle;
    winrt::Windows::System::Threading::ThreadPoolTimer _timer;
    

    void OnTick(const winrt::Windows::System::Threading::ThreadPoolTimer& timer);
  };
}