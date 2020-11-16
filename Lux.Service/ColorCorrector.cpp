#include "pch.h"
#include "ColorCorrector.h"

using namespace DirectX;
using namespace std;
using namespace Lux::Graphics;
using namespace winrt::Windows::Foundation::Numerics;

namespace Lux::Colors
{
  ColorCorrector::ColorCorrector()
  {
    Gamma({ 1.6f, 1.5f, 1.6f });
    Saturation(1.f);
  }

  float3 ColorCorrector::Gamma() const
  {
    return _gamma;
  }

  void ColorCorrector::Gamma(const float3& value)
  {
    _gamma = value;
    RebuildGamma();
  }

  float ColorCorrector::Saturation() const
  {
    return _saturation;
  }

  void ColorCorrector::Saturation(float value)
  {
    _saturation = value;
  }

  float ColorCorrector::Brightness() const
  {
    return _brightness;
  }

  void ColorCorrector::Brightness(float value)
  {
    _brightness = value;
    RebuildGamma();
  }

  float ColorCorrector::MaxBrightness() const
  {
    return _maxBrightness;
  }

  void ColorCorrector::MaxBrightness(float value)
  {
    _maxBrightness = value;
  }

  void ColorCorrector::RebuildGamma()
  {
    _gammaMappingR = make_gamma(_gamma.x, _brightness);
    _gammaMappingG = make_gamma(_gamma.y, _brightness);
    _gammaMappingB = make_gamma(_gamma.z, _brightness);
  }

  void ColorCorrector::ProcessColors(vector<rgb>& colors)
  {
    //Brightness limit & saturation adjustment
    auto saturation = 1.f / (4.f * _saturation);

    XMVECTOR sum{};
    for (auto& color : colors)
    {
      if (saturation != 1.f)
      {
        hsl hslColor = color;
        hslColor.s = saturation < 10 ? pow(hslColor.s, saturation) : 0.f ;
        color = hslColor;
      }

      sum = XMVectorAdd(XMVectorSet(
        _gammaMappingR[color.r],
        _gammaMappingG[color.g],
        _gammaMappingB[color.b],
        0.f
      ), sum);
    }

    auto brightness = XMVectorGetX(XMVectorSum(sum)) / 3.f / 255.f / colors.size();
    auto factor = brightness > _maxBrightness ? _maxBrightness / brightness : 1.f;

    //Gamma correct colors and perform dithering
    XMVECTOR target{}, error{}, actual{}, correction;
    XMFLOAT3 result;
    for (auto& color : colors)
    {
      target = XMVectorScale(XMVectorSet(
        _gammaMappingR[color.r],
        _gammaMappingG[color.g],
        _gammaMappingB[color.b],
        0.f
      ), factor);

      actual = XMVectorRound(target);

      error = XMVectorAdd(XMVectorSubtract(target, actual), error);

      correction = XMVectorRound(error);

      actual = XMVectorAdd(actual, correction);
      error = XMVectorSubtract(error, correction);

      XMStoreFloat3(&result, actual);
      color.r = uint8_t(result.x);
      color.g = uint8_t(result.y);
      color.b = uint8_t(result.z);
    }
  }  
}
