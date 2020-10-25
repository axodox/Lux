#pragma once
#include "ObservableValue.h"
#include "Serializer.h"

namespace Lux::Observable
{
  struct object_property_update_change : public change
  {
    uint16_t key;
    std::unique_ptr<change> value;

    virtual change_type type() const
    {
      return change_type::object_property_update;
    }
  };

  class observable_property_base
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
    observable_property(const observable::callback_t& callback, std::function<void(observable_property_base*)>&& initialize, uint16_t key, const TValue& value) :
      observable_value<TValue>(callback, value),
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
  };

  template <typename TPropertyKey, typename = std::enable_if_t<std::is_same<std::underlying_type_t<TPropertyKey>, uint16_t>::value>>
  class observable_object : public observable, public Serialization::serializable
  {
  public:
    typedef TPropertyKey key_t;

  private:
    const Events::event_owner _events;
    std::unordered_map<key_t, observable_property_base*> _properties;

  public:
    Events::event_publisher<observable_object*, key_t> property_changed;

    observable_object(const callback_t& callback) :
      observable(callback),
      property_changed(_events)
    { }

    template<typename T>
    observable_property<T> make_property(key_t key, const T& value = {})
    {
      return observable_property<T>([this, key](std::unique_ptr<change>&& change) {
        auto propertyChange = std::make_unique<object_property_update_change>();
        propertyChange->key = (uint16_t)key;
        propertyChange->value = std::move(change);
        report_change(std::move(propertyChange));

        _events.raise(property_changed, this, key);
        },
        [this, key](observable_property_base* property) { _properties.emplace(key, property); },
          (uint16_t)key,
          value);
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
      for (auto& [key, value] : _properties)
      {
        stream.write(key);
        stream.write(value);
      }
    }

    virtual void deserialize(Serialization::stream& stream) override
    {
      auto length = stream.read<uint32_t>();
      _properties.reserve(length);
      for (auto i = 0u; i < length; i++)
      {
        auto key = stream.read<key_t>();
        stream.read(_properties.at(key));
      }
    }
  };
}