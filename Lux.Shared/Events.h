#pragma once
#include "shared_pch.h"
#include "ResetEvent.h"

namespace Lux::Events
{
  template <typename TInstance, typename TMemberFunc>
  static auto member_func(TInstance&& instance, TMemberFunc&& memberFunction)
  {
    return[member = std::mem_fn(memberFunction), instance](auto&&... args){ return member(*instance, std::forward<decltype(args)>(args)...); };
  }

  template <typename TFunc, typename... TBoundArgs>
  static auto bind_func(TFunc&& func, TBoundArgs... boundArgs)
  {
    return[function = std::mem_fn(func), boundArgs...](auto&&... args){ 
      return function(boundArgs..., std::forward<decltype(args)>(args)...);
    };
  }

  class event_handler_collection_base
  {
  public:
    typedef uint32_t token_t;

    virtual bool remove(token_t token) noexcept = 0;

    virtual ~event_handler_collection_base() noexcept = default;
  };

  class event_subscription
  {
  private:
    std::weak_ptr<event_handler_collection_base> _event_handler_collection;
    event_handler_collection_base::token_t _token;

  public:
    event_subscription& operator=(event_subscription&& other) noexcept
    {
      _event_handler_collection = std::move(other._event_handler_collection);
      _token = other._token;
      other._token = event_handler_collection_base::token_t{};
    }

    event_subscription(event_subscription&& other) noexcept
    {
      *this = std::move(other);
    }

    event_subscription(const event_subscription&) = delete;
    event_subscription& operator=(const event_subscription&) = delete;

    event_subscription(const std::shared_ptr<event_handler_collection_base>& eventHandlerCollection, event_handler_collection_base::token_t token) noexcept :
      _event_handler_collection(eventHandlerCollection),
      _token(token)
    { }

    ~event_subscription() noexcept
    {
      auto eventHandlerCollection = _event_handler_collection.lock();
      if (eventHandlerCollection)
      {
        eventHandlerCollection->remove(_token);
      }
    }

    bool is_valid() const noexcept
    {
      return _event_handler_collection.lock() != nullptr;
    }
  };

  template<typename... TArgs>
  class event_handler_collection : public event_handler_collection_base
  {
  public:
    typedef std::function<void(TArgs...)> handler_t;

  private:
    std::shared_mutex _mutex;
    token_t _next_token = {};
    std::unordered_map<token_t, handler_t> _handlers;

  public:
    token_t add(handler_t&& handler) noexcept
    {
      std::unique_lock<std::shared_mutex> lock(_mutex);

      auto token = _next_token++;
      _handlers[token] = std::move(handler);
      return token;
    }

    virtual bool remove(token_t token) noexcept override
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

  struct no_revoke_t {};
  inline constexpr no_revoke_t no_revoke{};

  template<typename... TArgs>
  class event_publisher
  {
    friend class event_owner;

  private:
    size_t _owner_id;
    std::shared_ptr<event_handler_collection<TArgs...>> _handlers;

    void raise(const event_owner& owner, TArgs... args)
    {
      if (owner != _owner_id)
      {
        throw std::exception("The specified event owner does not own this event.");
      }

      _handlers->invoke(std::forward<TArgs>(args)...);
    }

  public:
    event_publisher(event_publisher&&) = default;
    event_publisher& operator=(event_publisher&&) = default;

    event_publisher(const event_publisher&) = delete;
    event_publisher& operator=(const event_publisher&) = delete;

    event_publisher(const event_owner& owner) noexcept :
      _owner_id(owner),
      _handlers(std::make_shared<event_handler_collection<TArgs...>>())
    { }

    [[nodiscard("Do not discard event subscription.")]]
    event_subscription subscribe(std::function<void(TArgs...)>&& handler) noexcept
    {
      return { _handlers, _handlers->add(std::move(handler)) };
    }

    void subscribe(no_revoke_t, std::function<void(TArgs...)>&& handler) noexcept
    {
      _handlers->add(std::move(handler));
    }

    [[nodiscard("Do not discard event subscription.")]]
    event_subscription operator()(std::function<void(TArgs...)>&& handler) noexcept
    {
      return subscribe(std::move(handler));
    }

    void operator()(no_revoke_t, std::function<void(TArgs...)>&& handler) noexcept
    {
      subscribe(no_revoke, std::move(handler));
    }

    std::tuple<TArgs...> wait(std::chrono::duration<uint32_t, std::milli> timeout)
    {
      return wait(timeout.count());
    }

    std::tuple<TArgs...> wait(uint32_t timeout = INFINITE)
    {
      if (timeout == 0u) return {};

      std::tuple<TArgs...> result{};

      Threading::manual_reset_event awaiter;
      auto subscription = subscribe([&](TArgs... args) -> void {
        result = { args... };
        awaiter.set();
        });

      awaiter.wait(timeout);
      return result;
    }

    ~event_publisher() noexcept
    {
      _handlers.reset();
    }
  };

  class event_owner
  {
  private:
    static inline size_t _next_id = 1;
    size_t _id;
    
  public:
    event_owner() noexcept :
      _id(_next_id++)
    { }

    event_owner& operator=(event_owner&& other) noexcept
    {
      _id = other._id;
      other._id = size_t{};
    }

    event_owner(event_owner&& other) noexcept
    {
      *this = std::move(other);
    }

    event_owner(const event_owner&) = delete;
    event_owner& operator=(const event_owner&) = delete;

    template<typename... TArgs, typename TEvent = event_publisher<TArgs...>>
    void raise(TEvent& event, TArgs... args) const
    {
      event.raise(*this, std::forward<TArgs>(args)...);
    }

    bool is_valid() const noexcept
    {
      return _id != size_t{};
    }

    operator size_t() const noexcept
    {
      return _id;
    }
  };

  }