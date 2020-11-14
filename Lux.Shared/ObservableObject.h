#pragma once
#include "ObservableValue.h"
#include "Serializer.h"

namespace Lux::Observable
{
  struct object_property_update_change : public change
  {
    uint16_t key;
    std::unique_ptr<change> value;

    virtual change_type type() const;

    virtual void serialize(Serialization::stream& stream) const override;
    virtual void deserialize(Serialization::stream& stream) override;
  };

  class observable_property_base : public virtual Serialization::serializable
  {
  public:
    virtual uint16_t key() const = 0;

    virtual void apply_change(change* change) = 0;
  };

  template<typename TValue>
  class observable_property : public observable_value<TValue>, public observable_property_base
  {
  private:
    const uint16_t _key;

  public:
    using observable_value<TValue>::operator TValue;
    using observable_value<TValue>::operator const TValue&;
    using observable_value<TValue>::operator TValue&;
    using observable_value<TValue>::operator=;
    using observable_value<TValue>::operator==;

    template<typename = std::enable_if_t<std::negation<std::is_convertible<TValue*, observable*>>::value>>
    observable_property(const observable::callback_t& callback, std::function<void(observable_property_base*)>&& initialize, uint16_t key, const TValue& value) :
      observable_value<TValue>(callback, value),
      _key(key)
    {
      initialize(this);
    }

    template<typename = std::enable_if_t<std::is_convertible<TValue*, observable*>::value>>
    observable_property(const observable::callback_t& callback, std::function<void(observable_property_base*)>&& initialize, uint16_t key) :
      observable_value<TValue>(callback),
      _key(key)
    {
      initialize(this);
    }

    virtual uint16_t key() const override
    {
      return _key;
    }

    virtual void apply_change(change* change) override
    {
      observable_value<TValue>::apply_change(change);
    }

#pragma warning ( suppress: 4250)
  };

  template <typename TPropertyKey, typename = std::enable_if_t<std::is_same<std::underlying_type_t<TPropertyKey>, uint16_t>::value>>
  class observable_object : public observable, public Serialization::serializable
  {
  public:
    typedef TPropertyKey key_t;
    typedef std::enable_if_t<std::is_same<std::underlying_type_t<TPropertyKey>, uint16_t>::value> enabled_t;

  private:
    const Events::event_owner _events;
    std::unordered_map<key_t, observable_property_base*> _properties;

    void on_child_added(uint16_t key, observable_property_base* property)
    {
      _properties.emplace((key_t)key, property);
    }

    void on_child_changed(uint16_t key, std::unique_ptr<change>&& change)
    {
      auto propertyChange = std::make_unique<object_property_update_change>();
      propertyChange->key = (uint16_t)key;
      propertyChange->value = std::move(change);
      report_change(std::move(propertyChange));
      _events.raise(property_changed, this, (key_t)key);
    }

  public:
    Events::event_publisher<observable_object*, key_t> property_changed;

    observable_object(const callback_t& callback) :
      observable(callback),
      property_changed(_events)
    { }

    template<typename T, typename = std::enable_if_t<std::negation<std::is_convertible<T*, observable*>>::value>>
    observable_property<T> make_property(key_t key, const T& value = {})
    {
      return observable_property<T>(
        Events::bind_func(&observable_object<key_t, enabled_t>::on_child_changed, this, (uint16_t)key),
        Events::bind_func(&observable_object<key_t, enabled_t>::on_child_added, this, (uint16_t)key),
        (uint16_t)key,
        value);
    }

    template<typename T, typename = std::enable_if_t<std::is_convertible<T*, observable*>::value>>
    observable_property<T> make_property(key_t key)
    {
      return observable_property<T>(
        Events::bind_func(&observable_object<key_t, enabled_t>::on_child_changed, this, (uint16_t)key),
        Events::bind_func(&observable_object<key_t, enabled_t>::on_child_added, this, (uint16_t)key),
        (uint16_t)key);
    }

    void apply_change(change* change)
    {
      if (change->type() != change_type::object_property_update)
      {
        throw std::exception("This object does not support the specified change type!");
      }

      auto propertyUpdate = static_cast<object_property_update_change*>(change);
      _properties.at((key_t)propertyUpdate->key)->apply_change(propertyUpdate->value.get());
    }

    virtual void serialize(Serialization::stream& stream) const override
    {
      stream.write((uint32_t)_properties.size());
      for (auto& [key, property] : _properties)
      {
        stream.write(key);
        stream.write(*property);
      }
    }

    virtual void deserialize(Serialization::stream& stream) override
    {
      auto length = stream.read<uint32_t>();
      _properties.reserve(length);
      for (auto i = 0u; i < length; i++)
      {
        auto key = stream.read<key_t>();
        stream.read(*_properties.at(key));
      }
    }
  };
}