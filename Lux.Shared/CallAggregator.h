#pragma once
#include "shared_pch.h"
#include "ThreadName.h"

namespace Lux::Threading
{
  template<typename... TArgs>
  class call_aggregator
  {
  private:
    std::function<void(TArgs...)> _target;
    std::tuple<TArgs...> _args;
    std::mutex _mutex;
    std::future<void> _pending;
    std::chrono::milliseconds _interval;
    std::chrono::steady_clock::time_point _lastCall;
    std::wstring _name;

  public:
    call_aggregator(std::function<void(TArgs...)> target, std::chrono::milliseconds interval, const std::wstring& name = L"call aggregator") :
      _target(target),
      _interval(interval),
      _lastCall({}),
      _name(name)
    { }

    void call(TArgs... args)
    {
      std::lock_guard<std::mutex> lock(_mutex);
      _args = std::forward_as_tuple(args...);
      if (!_pending.valid() || _pending.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
      {
        _pending = std::async(std::launch::async, [this] {
          Threading::thread_name_context name(L"* " + _name);

          std::this_thread::sleep_until(_lastCall + _interval);
          _lastCall = std::chrono::steady_clock::now();

          std::lock_guard<std::mutex> lock(_mutex);
          try
          {
            std::apply(_target, _args);
          }
          catch (...)
          {
            //log
          }
        });
      }
    }
  };
}
