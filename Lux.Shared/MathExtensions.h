#pragma once
#include "shared_pch.h"

namespace Lux::Math
{
  template<typename T>
  T mod(T value, T length) { return value - length * floor(value / length); }

  template<typename T>
  T wrap(T value, T min, T max)
  {
    return mod(value - min, max - min) + min;
  }

  template<typename T>
  T rad(T value) {
    return value * T(M_PI) / 180;
  }

  template<typename T>
  T deg(T value) {
    return value * 180 / T(M_PI);
  }
}