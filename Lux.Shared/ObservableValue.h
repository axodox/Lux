#pragma once
#include "Observable.h"
#include "Events.h"
#include "MemoryStream.h"

namespace Lux::Observable
{
  struct value_update_change : public change
  {
    Serialization::memory_stream data;

    virtual change_type type() const;

    virtual void serialize(Serialization::stream& stream) const override;
    virtual void deserialize(Serialization::stream& stream) override;
  };

  template<typename TValue>
  class observable_value : public observable, public virtual Serialization::serializable
  {
  public:
    typedef TValue value_t;

  private:
    const Events::event_owner _events;
    value_t _value;

    template<typename = std::enable_if_t<std::negation<std::is_convertible<value_t*, observable*>>::value>>
    void on_changed()
    {
      auto change = std::make_unique<value_update_change>();
      change->data.write(_value);
      report_change(std::move(change));

      _events.raise(changed, this, _value);
    }

  public:
    Events::event_publisher<observable_value<value_t>*, const value_t&> changed;

    template<typename = std::enable_if_t<std::negation<std::is_convertible<value_t*, observable*>>::value>>
    observable_value(const callback_t& callback, const value_t& value = {}) :
      observable(callback),
      _value(value),
      changed(_events)
    { }

    template<typename = std::enable_if_t<std::is_convertible<value_t*, observable*>::value>>
    observable_value(const callback_t& callback) :
      observable(callback),
      _value(callback),
      changed(_events)
    { }

    const value_t& value() const
    {
      return _value;
    }

    template<typename = std::enable_if_t<std::is_convertible<value_t*, observable*>::value>>
    value_t& value()
    {
      return _value;
    }

    template<typename = std::enable_if_t<std::negation<std::is_convertible<value_t*, observable*>>::value>>
    operator value_t() const
    {
      return _value;
    }

    operator const value_t&() const
    {
      return _value;
    }

    template<typename = std::enable_if_t<std::is_convertible<value_t*, observable*>::value>>
    operator value_t&()
    {
      return _value;
    }

    const value_t& operator*() const
    {
      return _value;
    }

    template<typename = std::enable_if_t<std::is_convertible<value_t*, observable*>::value>>
    value_t& operator*()
    {
      return _value;
    }

    const value_t* operator->() const
    {
      return &_value;
    }

    template<typename = std::enable_if_t<std::is_convertible<value_t*, observable*>::value>>
    value_t* operator->()
    {
      return &_value;
    }

    template<typename = std::enable_if_t<std::negation<std::is_convertible<value_t*, observable*>>::value>>
    void value(const value_t& value)
    {
      _value = value;
      on_changed();
    }

    template<typename = std::enable_if_t<std::negation<std::is_convertible<value_t*, observable*>>::value>>
    const value_t& operator=(const value_t& value)
    {
      _value = value;
      on_changed();
      return _value;
    }

    template<typename = std::enable_if_t<std::negation<std::is_convertible<value_t*, observable*>>::value>>
    void value(value_t&& value)
    {
      _value = std::move(value);
      on_changed();
    }

    template<typename = std::enable_if_t<std::negation<std::is_convertible<value_t*, observable*>>::value>>
    const value_t& operator=(value_t&& value)
    {
      _value = std::move(value);
      on_changed();
      return _value;
    }

    template<typename = std::enable_if_t<std::negation<std::is_convertible<value_t*, observable*>>::value>>
    bool operator ==(const value_t& other)
    {
      return _value == other;
    }

    virtual void apply_change(change* change) override
    {
      if constexpr (std::is_convertible<value_t*, observable*>::value)
      {
        _value.apply_change(change);
      }
      else
      {
        if (change->type() == change_type::value_update)
        {
          auto valueUpdate = static_cast<value_update_change*>(change);
          valueUpdate->data.read(_value);
          on_changed();
        }
        else
        {
          throw std::exception("This object does not support the specified change type!");
        }
      }
    }

    virtual void serialize(Serialization::stream& stream) const override
    {
      stream.write(_value);
    }

    virtual void deserialize(Serialization::stream& stream) override
    {
      stream.read(_value);
    }
  };
}