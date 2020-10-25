#pragma once
#include "ResetEvent.h"

namespace Lux::Threading
{
  template <typename TAsync>
  void await_operation(TAsync const& async, bool processEvents = false)
  {
    using namespace std::chrono_literals;

    ::Lux::Threading::manual_reset_event completed;
    async.Completed([&](auto&&...)
      {
        completed.set();
      });

    if (processEvents)
    {
#ifdef UWP_UI
      auto window = winrt::Windows::UI::Core::CoreWindow::GetForCurrentThread();
      if (window)
      {
        bool firstRun = true;
        //This is needed when called on UI std::thread since some calls will use the UI std::thread for execution, which would otherwise cause a deadlock
        while (!completed.wait(0))
        {
          window.Dispatcher().ProcessEvents(winrt::Windows::UI::Core::CoreProcessEventsOption::ProcessAllIfPresent);

          //Handle synchronous execution
          if (firstRun)
          {
            firstRun = false;
          }
          else
          {
            std::this_thread::sleep_for(5ms);
          }
        };
      }
#endif
    }
    completed.wait();
  }

  template <typename TAsync>
  auto await_results(TAsync const& async, bool processEvents = false)
  {
    await_operation(async, processEvents);
    return async.GetResults();
  }
}