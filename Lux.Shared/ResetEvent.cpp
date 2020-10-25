#include "pch.h"
#include "ResetEvent.h"

namespace Lux::Threading
{
  uint32_t reset_event_base::wait_many(const std::initializer_list<reset_event_base*>& events, bool waitAll, uint32_t timeout) noexcept
  {
    std::vector<void*> handles;
    handles.reserve(events.size());
    for (auto event : events)
    {
      if(event->is_valid()) handles.push_back(event->_event.get());
    }

    auto result = WaitForMultipleObjects(uint32_t(handles.size()), handles.data(), waitAll, timeout);
    if (result == WAIT_TIMEOUT)
    {
      return uint32_t(-1);
    }
    else
    {
      return result;
    }
  }
  
  bool reset_event_base::is_valid() const noexcept
  {
    return _event.get() != nullptr;
  }
    
  bool reset_event_base::wait(uint32_t timeout) const noexcept
  {
    if (_event)
    {
      return WaitForSingleObject(_event.get(), timeout) == WAIT_OBJECT_0;
    }
    else
    {
      return true;
    }
  }
  
  bool reset_event_base::wait(std::chrono::milliseconds timeout) const noexcept
  {
    return wait((uint32_t)timeout.count());
  }
  
  void reset_event_base::set() noexcept
  {
    if (_event) SetEvent(_event.get());
  }
  
  void reset_event_base::reset()
  {
    if (_event) ResetEvent(_event.get());
    else throw std::exception("Cannot reset a blank event.");
  }
  
  uint32_t reset_event_base::wait_any(const std::initializer_list<reset_event_base*>& events, uint32_t timeout) noexcept
  {
    return wait_many(events, false, timeout);
  }

  uint32_t reset_event_base::wait_any(const std::initializer_list<reset_event_base*>& events, std::chrono::milliseconds timeout) noexcept
  {
    return wait_many(events, false, uint32_t(timeout.count()));
  }
}