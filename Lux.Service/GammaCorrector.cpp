#include "pch.h"
#include "GammaCorrector.h"

using namespace std;
using namespace Lux::Graphics;
using namespace winrt::Windows::Foundation::Numerics;

namespace Lux::Colors
{
  GammaCorrector::GammaCorrector()
  {
    Gamma({ 1.6f, 1.5f, 1.6f });
  }

  float3 GammaCorrector::Gamma() const
  {
    return _gamma;
  }

  void GammaCorrector::Gamma(const float3& value)
  {
    _gamma = value;
    _gammaMappingR = make_gamma(value.x);
    _gammaMappingG = make_gamma(value.y);
    _gammaMappingB = make_gamma(value.z);
  }

  void GammaCorrector::ProcessColors(vector<rgb>& colors)
  {
    //Gamma correct colors and perform dithering
    float3 target{}, error{}, actual{}, correction;
    for (auto& color : colors)
    {
      target = {
        _gammaMappingR[color.r],
        _gammaMappingG[color.g],
        _gammaMappingB[color.b]
      };

      actual = {
        round(target.x),
        round(target.y),
        round(target.z),
      };

      error += target - actual;

      correction = {
        round(error.x),
        round(error.y),
        round(error.z)
      };

      actual += correction;
      error -= correction;

      color.r = (uint8_t)actual.x;
      color.g = (uint8_t)actual.y;
      color.b = (uint8_t)actual.z;
    }
  }
}
