#pragma once
#include "pch.h"

namespace Lux::Events
{
  class event_handler_collection_base
  {
  public:
    typedef uint32_t token_t;

    virtual bool remove(token_t token) = 0;

    virtual ~event_handler_collection_base() = default;
  };

  class event_subscription
  {
  private:
    const std::weak_ptr<event_handler_collection_base> _event;
    const event_handler_collection_base::token_t _token;

  public:
    event_subscription(const std::shared_ptr<event_handler_collection_base>& event, event_handler_collection_base::token_t token) :
      _event(event),
      _token(token)
    { }

    ~event_subscription()
    {
      auto event = _event.lock();
      if (event)
      {
        event->remove(_token);
      }
    }
  };

  template<typename... TArgs>
  class event_handler_collection : public event_handler_collection_base
  {
  public:
    typedef std::function<void(TArgs...)> handler_t;

  private:
    std::shared_mutex _mutex;
    token_t _nextToken = {};
    std::unordered_map<token_t, handler_t> _handlers;

  public:
    token_t add(handler_t&& handler)
    {
      std::unique_lock<std::shared_mutex> lock(_mutex);

      auto token = _nextToken++;
      _handlers[token] = std::move(handler);
      return token;
    }

    virtual bool remove(token_t token) override
    {
      std::unique_lock<std::shared_mutex> lock(_mutex);
      return _handlers.erase(token);
    }

    void invoke(TArgs... args)
    {
      std::shared_lock<std::shared_mutex> lock(_mutex);
      for (auto& [token, handler] : _handlers)
      {
        handler(std::forward<TArgs>(args)...);
      }
    }
  };

  template<typename... TArgs>
  class event_publisher
  {
    friend struct event_owner;

  private:
    const event_owner* _owner;
    std::shared_ptr<event_handler_collection<TArgs...>> _handlers;

    void raise(const event_owner& owner, TArgs... args)
    {
      if (&owner != _owner)
      {
        throw std::exception("The specified event owner does not own this event.");
      }

      _handlers->invoke(std::forward<TArgs>(args)...);
    }

  public:
    event_publisher(const event_owner& owner) :
      _owner(&owner),
      _handlers(std::make_shared<event_handler_collection<TArgs...>>())
    { }

    [[nodiscard("Do not discard event subscription.")]]
    event_subscription subscribe(std::function<void(TArgs...)>&& handler)
    {
      return { _handlers, _handlers->add(std::move(handler)) };
    }

    [[nodiscard("Do not discard event subscription.")]]
    event_subscription operator()(std::function<void(TArgs...)>&& handler)
    {
      return subscribe(std::move(handler));
    }

    ~event_publisher()
    {
      _handlers.reset();
    }
  };

  struct event_owner
  {
    event_owner() = default;

    event_owner(const event_owner&) = delete;
    event_owner(event_owner&&) = delete;

    event_owner& operator=(const event_owner&) = delete;
    event_owner& operator=(event_owner&&) = delete;

    template<typename... TArgs, typename TEvent = event_publisher<TArgs...>>
    void raise(TEvent& event, TArgs... args) const
    {
      event.raise(*this, std::forward<TArgs>(args)...);
    }
  };
}