#pragma once
#include "ColorProcessor.h"

namespace Lux::Colors
{
  class GammaCorrector : public ColorProcessor
  {
  public:
    GammaCorrector();

    winrt::Windows::Foundation::Numerics::float3 Gamma() const;
    void Gamma(const winrt::Windows::Foundation::Numerics::float3& value);

    virtual void ProcessColors(std::vector<Graphics::rgb>& colors) override;

  private:
    winrt::Windows::Foundation::Numerics::float3 _gamma;
    std::array<float, 256> _gammaMappingR;
    std::array<float, 256> _gammaMappingG;
    std::array<float, 256> _gammaMappingB;
  };
}