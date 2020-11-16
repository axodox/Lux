#pragma once
#include "ColorProcessor.h"

namespace Lux::Colors
{
  class ColorCorrector : public ColorProcessor
  {
  public:
    ColorCorrector();

    winrt::Windows::Foundation::Numerics::float3 Gamma() const;
    void Gamma(const winrt::Windows::Foundation::Numerics::float3& value);

    float Saturation() const;
    void Saturation(float value);

    float Brightness() const;
    void Brightness(float value);

    float MaxBrightness() const;
    void MaxBrightness(float value);

    virtual void ProcessColors(std::vector<Graphics::rgb>& colors) override;

  private:
    float _brightness = 1.f;
    float _saturation = 1.f;
    float _maxBrightness = 0.7f;
    winrt::Windows::Foundation::Numerics::float3 _gamma;

    std::array<float, 256> _gammaMappingR;
    std::array<float, 256> _gammaMappingG;
    std::array<float, 256> _gammaMappingB;

    void RebuildGamma();
  };
}