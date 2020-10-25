#pragma once
#include "shared_pch.h"
#include "ResetEvent.h"

namespace Lux::Threading
{
  template <typename TItem>
  class blocking_collection
  {
  public:
    typedef TItem item_t;

  private:
    bool _is_complete = false;
    std::mutex _mutex;
    std::queue<item_t> _queue;
    Threading::auto_reset_event _event;
    size_t _max_size;

  public:
    explicit blocking_collection(size_t maxSize = std::numeric_limits<size_t>::max()) :
      _max_size(maxSize)
    { }

    void clear()
    {
      std::lock_guard<std::mutex> lock(_mutex);
      _queue = {};
    }

    void add(item_t&& item)
    {
      auto result = try_add(std::move(item));
      if (!result) throw std::exception("Cannot add after the collection is complete.");
    }

    bool try_add(item_t&& item)
    {
      std::lock_guard<std::mutex> lock(_mutex);
      if (!_is_complete)
      {
        _queue.push(std::move(item));
        if (_queue.size() > _max_size) _queue.pop();
        _event.set();
        return true;
      }
      else
      {
        return false;
      }      
    }

    void complete()
    {
      std::lock_guard<std::mutex> lock(_mutex);
      _is_complete = true;
      _event.set();
    }

    bool is_complete() const
    {
      return _is_complete;
    }

    size_t size()
    {
      std::lock_guard<std::mutex> lock(_mutex);
      return _queue.size();
    }

    bool try_get(item_t& item, uint32_t timeout = INFINITE)
    {
      for (auto i = 0u; i < 2u; i++)
      {
        {
          std::lock_guard<std::mutex> lock(_mutex);
          if (_queue.size() > 0)
          {
            if (_queue.size() == 1) _event.reset();
            item = std::move(_queue.front());
            _queue.pop();
            return true;
          }
          else if (i == 1u || _is_complete)
          {
            return false;
          }
        }

        _event.wait(timeout);
      }

      return false; //This should never run
    }

    bool try_get(item_t& item, std::chrono::milliseconds timeout)
    {
      return try_get(item, (uint32_t)timeout.count());
    }

    ~blocking_collection()
    {
      complete();
    }
  };
}
