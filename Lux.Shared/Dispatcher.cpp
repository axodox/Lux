#include "pch.h"
#include "Dispatcher.h"

using namespace std;
using namespace winrt;
using namespace winrt::Windows::UI::Core;

namespace Lux::Threading
{
  simple_dispatcher::simple_dispatcher() :
    _thread([&] { run(); }, L"simple dispatcher")
  { }

  simple_dispatcher::~simple_dispatcher()
  {
    _tasks.complete();
  }

  std::future<void> simple_dispatcher::invoke(std::function<void()>&& callback)
  {
    dispatched_task task{};
    task.callback = move(callback);
    auto future = task.promise.get_future();

    if (_thread.is_current())
    {
      execute_task(task);
    }
    else
    { 
      _tasks.add(move(task));
    }

    return future;
  }

  bool simple_dispatcher::has_access() const
  {
    return _thread.is_current();
  }

  void simple_dispatcher::execute_task(dispatched_task& task)
  {
    try
    {
      task.callback();
      task.promise.set_value();
    }
    catch (...)
    {
      task.promise.set_exception(current_exception());
    }
  }

  void simple_dispatcher::run()
  {
    while (!_tasks.is_complete())
    {
      dispatched_task task;
      if (_tasks.try_get(task))
      {
        execute_task(task);
      }
    }
  }

  core_dispatcher::core_dispatcher(const winrt::Windows::UI::Core::CoreDispatcher& dispatcher) :
    _dispatcher(dispatcher)
  { }

  std::future<void> core_dispatcher::invoke(std::function<void()> && callback)
  {
    auto promise = make_unique<std::promise<void>>();
    auto future = promise->get_future();

    if (_dispatcher.HasThreadAccess())
    {
      try
      {
        callback();
        promise->set_value();
      }
      catch (...)
      {
        promise->set_exception(current_exception());
      }
    }
    else
    {
      _dispatcher
        .RunAsync(CoreDispatcherPriority::Normal, [callback = move(callback)]{ callback(); })
        .Completed([promise = move(promise)](auto&, auto&) { promise->set_value(); });
    }

    return future;
  }
  
  bool core_dispatcher::has_access() const
  {
    return _dispatcher.HasThreadAccess();
  }
}
