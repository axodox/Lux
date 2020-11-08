#pragma once
#include "shared_pch.h"

namespace Lux::Graphics
{
  struct rgb
  {
    uint8_t r, g, b;

    rgb() = default;
    rgb(uint8_t r, uint8_t g, uint8_t b);
    rgb(const winrt::Windows::Foundation::Numerics::float3& color);
  };

  struct hsl
  {
    float h, s, l;

    hsl() = default;
    hsl(float h, float s, float l);
    hsl(const winrt::Windows::Foundation::Numerics::float3 & color);
  };

  std::array<float, 256> make_gamma(float gamma, float min = 0.2f);
}