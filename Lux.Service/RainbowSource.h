#pragma once
#include "LightSource.h"
#include "BackgroundThread.h"

namespace Lux::Sources
{
  class RainbowSource : public LightSource
  {
  public:
    RainbowSource();

    uint8_t SpatialFrequency() const;
    void SpatialFrequency(uint8_t value);

    float AngularVelocity() const;
    void AngularVelocity(float value);

    virtual Configuration::LightSourceKind Kind() override;

  private:
    uint8_t _spatialFrequency;
    float _angularVelocity;
    float _lastAngle;
    Threading::background_thread _worker;
    
    void Worker();
  };
}