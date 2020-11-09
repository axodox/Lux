#pragma once
#include "shared_pch.h"

namespace Lux::Threading
{
  template<typename TValue>
  class locked_ptr;

  template<typename TValue>
  class locked_ref
  {
    template<typename TValue>
    friend class locked_ptr;

  public:
    typedef TValue value_t;

  private:
    std::shared_lock<std::shared_mutex> _lock;
    value_t* _value;

    locked_ref(locked_ptr<value_t>& ptr);

  public:
    const TValue& operator*() const noexcept
    {
      return *_value;
    }

    TValue* get() const noexcept
    {
      return _value;
    }

    TValue& operator*() noexcept
    {
      return *_value;
    }

    TValue* operator->() const noexcept
    {
      return _value;
    }

    template <typename TTo>
    TTo* as() const noexcept
    {
      return static_cast<TTo*>(_value);
    }

    TValue& operator[](size_t id) const
    {
      return _value[id];
    }

    bool operator==(const nullptr_t&) const noexcept
    {
      return _value == nullptr;
    }

    bool operator!=(const nullptr_t&) const noexcept
    {
      return _value != nullptr;
    }

    operator bool() const noexcept
    {
      return _value != nullptr;
    }
  };

  template<typename TValue>
  class locked_ptr
  {
    template<typename TValue>
    friend class locked_ref;

  public:
    typedef TValue value_t;

  private:
    std::shared_mutex _mutex;
    std::unique_ptr<value_t> _value;

  public:
    locked_ref<value_t> get() noexcept
    {
      return locked_ref<value_t>(*this);
    }

    locked_ptr() noexcept
    { }

    locked_ptr(value_t* value) noexcept
    {
      _value = std::unique_ptr<value_t>(value);
    }

    locked_ptr(std::unique_ptr<value_t>&& value) noexcept : locked_ptr(value.release())
    { }

    locked_ptr<value_t>& operator=(value_t* value) noexcept
    {
      std::unique_lock<std::shared_mutex> lock(_mutex);
      _value = std::unique_ptr<value_t>(value);
      return *this;
    }

    locked_ptr<value_t>& operator=(std::unique_ptr<value_t>&& value) noexcept
    {
      return operator=(value.release());
    }

    ~locked_ptr()
    {
      //Prevent dangling references
      std::unique_lock<std::shared_mutex> lock(_mutex);
    }
  };

  template<typename TValue>
  locked_ref<TValue>::locked_ref(locked_ptr<TValue>& ptr) :
    _lock{ ptr._mutex },
    _value{ ptr._value.get() }
  { }
}