#pragma once
#include "shared_pch.h"

namespace Lux::Threading
{
  struct reset_event_base
  {
  protected:
    winrt::handle _event;

    static uint32_t wait_many(const std::initializer_list<reset_event_base*>& events, bool waitAll, uint32_t timeout = INFINITE) noexcept;

  public:
    bool is_valid() const noexcept;

    bool wait(uint32_t timeout = INFINITE) const noexcept;
    bool wait(std::chrono::milliseconds timeout) const noexcept;

    void set() noexcept;
    void reset();

    static uint32_t wait_any(const std::initializer_list<reset_event_base*>& events, uint32_t timeout = INFINITE) noexcept;
    static uint32_t wait_any(const std::initializer_list<reset_event_base*>& events, std::chrono::milliseconds timeout) noexcept;

    virtual ~reset_event_base() = default;
  };

  template<bool CIsManual>
  struct reset_event : public reset_event_base
  {
  public:
    reset_event(bool isReady = false)
    {
      _event = winrt::handle(CreateEvent(nullptr, CIsManual, isReady, nullptr));
      winrt::check_pointer(_event.get());
    }

    reset_event(nullptr_t) noexcept
    { }
  };

  struct manual_reset_event : public reset_event<true>
  {
    using reset_event<true>::reset_event;
  };

  struct auto_reset_event : public reset_event<false>
  {
    using reset_event<false>::reset_event;
  };
}
