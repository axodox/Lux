#include "pch.h"
#include "SteadyTimer.h"

using namespace std;
using namespace std::chrono;
using namespace std::this_thread;

namespace Lux::Threading
{
  steady_timer::steady_timer(milliseconds interval) :
    _interval(interval),
    _time(steady_clock::now())
  { }

  milliseconds steady_timer::interval() const
  {
    return _interval;
  }

  void steady_timer::wait()
  {
    auto now = steady_clock::now();
    auto elapsedTime = now - _time;
    if (elapsedTime < _interval)
    {
      sleep_for(_interval - elapsedTime);
      _time += _interval;
    }
    else
    {
      _time = now;
    }
  }
}
