#pragma once
#include "shared_pch.h"

namespace Lux::Graphics
{
  struct hsl;

  struct rgb
  {
    uint8_t r, g, b;

    rgb() = default;
    rgb(uint8_t r, uint8_t g, uint8_t b);
    rgb(const winrt::Windows::Foundation::Numerics::float3& color);
    rgb(winrt::Windows::UI::Color color);

    operator hsl() const;
    operator winrt::Windows::UI::Color() const;
  };

  rgb lerp(const rgb& a, const rgb& b, float factor);

  struct hsl
  {
    float h, s, l; //h: [0..360], s: [0..1], l: [0..1]

    hsl() = default;
    hsl(float h, float s, float l);
    hsl(const winrt::Windows::Foundation::Numerics::float3 & color);

    operator rgb() const;
  };

  std::array<float, 256> make_gamma(float gamma, float max = 1.f, float min = 0.2f);
}