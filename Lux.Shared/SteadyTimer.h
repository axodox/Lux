#pragma once
#include "shared_pch.h"

namespace Lux::Threading
{
  class steady_timer
  {
  private:
    std::chrono::milliseconds _interval;
    std::chrono::steady_clock::time_point _time;

  public:
    steady_timer(std::chrono::milliseconds interval);

    std::chrono::milliseconds interval() const;

    void wait();
  };
}