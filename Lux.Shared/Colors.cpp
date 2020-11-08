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

  std::array<float, 256> make_gamma(float gamma, float min)
  {
    std::array<float, 256> values;
    for (auto i = 0u; i < 256; i++)
    {
      auto value = pow(i / 255.f, gamma) * 255.f;
      if (value < min) value = 0.f; //Ignore small values for better dithering results
      values[i] = value;
    }
    return values;
  }
}