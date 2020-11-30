#include "pch.h"
#include "Colors.h"

using namespace std;
using namespace winrt::Windows::Foundation::Numerics;

namespace Lux::Graphics
{
  rgb::rgb(uint8_t r, uint8_t g, uint8_t b) :
    r(r),
    g(g),
    b(b)
  { }

  rgb::rgb(const float3& color) :
    r(uint8_t(color.x * 255)),
    g(uint8_t(color.y * 255)),
    b(uint8_t(color.z * 255))
  { }

  rgb::rgb(winrt::Windows::UI::Color color) :
    r(color.R),
    g(color.G),
    b(color.B)
  { }

  rgb::operator hsl() const
  {
    hsl result;

    auto floatRGB = float3(r, g, b) / 255.f;

    auto max = std::max({ floatRGB.x, floatRGB.y, floatRGB.z });
    auto min = std::min({ floatRGB.x, floatRGB.y, floatRGB.z });

    auto diff = max - min;
    result.l = (max + min) / 2.f;
    if (abs(diff) < 0.00001f)
    {
      result.s = 0.f;
      result.h = 0.f;
    }
    else
    {
      if (result.l <= 0.5f)
      {
        result.s = diff / (max + min);
      }
      else
      {
        result.s = diff / (2.f - max - min);
      }

      auto dist = (float3(max) - floatRGB) / diff;

      if (floatRGB.x == max)
      {
        result.h = dist.z - dist.y;
      }
      else if (floatRGB.y == max)
      {
        result.h = 2.f + dist.x - dist.z;
      }
      else
      {
        result.h = 4.f + dist.y - dist.x;
      }

      result.h = result.h * 60.f;
      if (result.h < 0.f) result.h += 360.f;
    }

    return result;
  }

  rgb::operator winrt::Windows::UI::Color() const
  {
    return { 255ui8, r, g, b };
  }

  hsl::hsl(float h, float s, float l) :
    h(h),
    s(s),
    l(l)
  { }

  hsl::hsl(const winrt::Windows::Foundation::Numerics::float3& color) :
    h(color.x),
    s(color.y),
    l(color.z)
  { }

  float qqh_to_rgb(float q1, float q2, float hue)
  {
    if (hue > 360.f) hue -= 360.f;
    else if (hue < 0.f) hue += 360.f;

    if (hue < 60.f) return q1 + (q2 - q1) * hue / 60.f;
    if (hue < 180.f) return q2;
    if (hue < 240.f) return q1 + (q2 - q1) * (240.f - hue) / 60.f;
    return q1;
  }

  hsl::operator rgb() const
  {
    float p2;
    if (l <= 0.5f) p2 = l * (1 + s);
    else p2 = l + s - l * s;

    float p1 = 2.f * l - p2;
    float3 floatRGB;
    if (s == 0)
    {
      floatRGB = float3(l);
    }
    else
    {
      floatRGB = float3(
        qqh_to_rgb(p1, p2, h + 120.f),
        qqh_to_rgb(p1, p2, h),
        qqh_to_rgb(p1, p2, h - 120.f)
      );
    }

    floatRGB *= 255.f;
    return rgb{
      (uint8_t)floatRGB.x,
      (uint8_t)floatRGB.y,
      (uint8_t)floatRGB.z
    };
  }

  rgb lerp(const rgb& a, const rgb& b, float factor)
  {
    auto invFactor = 1 - factor;
    return {
      uint8_t(a.r * invFactor + b.r * factor),
      uint8_t(a.g * invFactor + b.g * factor),
      uint8_t(a.b * invFactor + b.b * factor)
    };
  }

  std::array<float, 256> make_gamma(float gamma, float max, float min)
  {
    std::array<float, 256> values;
    for (auto i = 0u; i < 256; i++)
    {
      auto value = pow(i / 255.f, gamma) * 255.f * max;
      if (value < min) value = 0.f; //Ignore small values for better dithering results
      values[i] = value;
    }
    return values;
  }
}