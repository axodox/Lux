#pragma once
#include "shared_pch.h"

namespace Lux::Math
{
  union rect
  {
    struct {
      float left, top, right, bottom;
    };

    struct {
      winrt::Windows::Foundation::Numerics::float2 top_left, bottom_right;
    };

    rect(float top, float left, float bottom, float right) :
      left(left),
      top(top),
      right(right),
      bottom(bottom)
    { }

    rect(winrt::Windows::Foundation::Numerics::float2 topLeft, winrt::Windows::Foundation::Numerics::float2 bottomRight) :
      left(topLeft.x),
      top(topLeft.y),
      right(bottomRight.x),
      bottom(bottomRight.y)
    { }

    bool intersects(const rect& other) const
    {
      return (left < other.right&& right > other.left && top > other.bottom && bottom < other.top);
    }

    winrt::Windows::Foundation::Numerics::float2 center() const
    {
      return (top_left + bottom_right) / 2.f;
    }
  };
}