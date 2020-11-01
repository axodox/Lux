#pragma once
#include "shared_pch.h"
#include "BackgroundThread.h"
#include "BlockingCollection.h"

namespace Lux::Threading
{
  class dispatcher
  {
  public:
    virtual std::future<void> invoke(std::function<void()>&& callback) = 0;

    virtual bool has_access() const = 0;

    virtual ~dispatcher() = default;
  };

  class simple_dispatcher : public dispatcher
  {
  public:
    simple_dispatcher();
    ~simple_dispatcher();

    virtual std::future<void> invoke(std::function<void()>&& callback) override;
    virtual bool has_access() const override;

  private:
    struct dispatched_task
    {
      std::function<void()> callback;
      std::promise<void> promise;
    };

    blocking_collection<dispatched_task> _tasks;
    background_thread _thread;

    void execute_task(dispatched_task& task);
    void run();
  };

  class core_dispatcher : public dispatcher
  {
  public:
    core_dispatcher(const winrt::Windows::UI::Core::CoreDispatcher& dispatcher);
    
    virtual std::future<void> invoke(std::function<void()>&& callback) override;
    virtual bool has_access() const override;

  private:
    winrt::Windows::UI::Core::CoreDispatcher _dispatcher;
  };
}