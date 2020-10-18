#pragma once
#include "Events.h"
#include "Serializer.h"
#include "MemoryStream.h"

namespace Lux::Data
{
  //value_change (property) -> property_change (object) -> property_change (object)

  enum class change_type
  {
    unknown,
    value_update,
    object_property_update,
    vector_item_insertion,
    vector_item_update,
    vector_item_removal
  };

  struct change
  {
    virtual change_type type() const = 0;

    virtual ~change() = default;
  };

  struct value_update_change : public change
  {
    Streams::memory_stream data;

    virtual change_type type() const
    {
      return change_type::value_update;
    }
  };

  struct object_property_update_change : public change
  {
    uint16_t key;
    std::unique_ptr<change> value;

    virtual change_type type() const
    {
      return change_type::object_property_update;
    }
  };

  struct vector_item_change : public change
  {
    uint32_t index;
  };

  struct vector_item_insertion_change : public vector_item_change
  {
    virtual change_type type() const
    {
      return change_type::vector_item_insertion;
    }
  };

  struct vector_item_update_change : public vector_item_change
  {
    std::unique_ptr<change> value;

    virtual change_type type() const
    {
      return change_type::vector_item_update;
    }
  };

  struct vector_item_removal_change : public vector_item_change
  {
    virtual change_type type() const
    {
      return change_type::vector_item_removal;
    }
  };

  class observable
  {
  public:
    typedef std::function<void(std::unique_ptr<change>&&)> callback_t;

  private:
    const callback_t _callback;

  protected:
    void report_change(std::unique_ptr<change>&& change)
    {
      _callback(std::move(change));
    }

  public:
    observable(const callback_t& callback) :
      _callback(callback)
    { }

    virtual void apply_change(change* change) = 0;
  };

  template<typename TValue>
  class observable_value : public observable
  {
  public:
    typedef TValue value_t;

  private:
    const Events::event_owner _events;
    value_t _value;

    void on_changed()
    {
      auto change = std::make_unique<value_update_change>();
      change->data.write(_value);
      report_change(std::move(change));

      _events.raise(changed, this, _value);
    }

  public:
    Events::event_publisher<observable_value<value_t>*, const value_t&> changed;

    observable_value(const callback_t& callback, const value_t& value = {}) :
      observable(callback),
      _value(value),
      changed(_events)
    { 
      if constexpr (std::is_convertible<value_t*, observable*>::value)
      {
        _value = std::make_unique<value_t>([callback](std::unique_ptr<change>&& change) {
          (*that)->_callback(*that, std::move(change));
        });
      }
      else
      {
        _value = std::make_unique<item_t>();
      }
    }

    value_t& value() const
    {
      return _value;
    }

    void value(const value_t& value)
    {
      _value = value;
      on_changed();
    }

    void value(value_t&& value)
    {
      _value = std::move(value);
      on_changed();
    }

    virtual void apply_change(change* change) override
    {
      if (change->type() != change_type::value_update) throw std::exception("This object does not support the specified change type!");

      auto valueUpdate = static_cast<value_update_change*>(change);
      valueUpdate->data.read(_value);
    }
  };

  template<typename TItem>
  class observable_vector : public observable, public Streams::serializable
  {
  public:
    typedef TItem item_t;
    Events::event_publisher<observable_vector<item_t>*, uint32_t> added, changed, removed;

  private:
    class item_container : public Streams::serializable
    {
      std::unique_ptr<item_t> _value;
      std::function<void(item_container*, std::unique_ptr<change>&&)> _callback;
      std::unique_ptr<item_container*> _this; //Needed to maintain the location of the container in the vector when it is resized

      void on_changed()
      {
        auto change = std::make_unique<value_update_change>();
        change->data.write(*_value);
        _callback(this, change);
      }

    public:
      item_container(const std::function<void(item_container*, std::unique_ptr<change>&&)>& callback) :
        _callback(callback),
        _this(std::make_unique<item_container*>(this))
      {
        if constexpr (std::is_convertible<item_t*, observable*>::value)
        {
          _value = std::make_unique<item_t>([that = _this.get()](std::unique_ptr<change>&& change) {
            (*that)->_callback(*that, std::move(change));
            });
        }
        else
        {
          _value = std::make_unique<item_t>();
        }
      }

      item_container(const item_container& other) = delete;
      item_container& operator=(const item_container& other) = delete;

      item_container(item_container&& other) noexcept
      {
        *this = std::move(other);
      }

      item_container& operator=(item_container&& other) noexcept
      {
        _value = std::move(other._value);
        _callback = std::move(other._callback);
        _this = std::move(other._this);
        *_this = this;
        return *this;
      }

      const item_t& value() const
      {
        return *_value;
      }

      template<typename = std::enable_if_t<std::is_convertible<item_t*, observable*>::value>>
      item_t& value()
      {
        return *_value;
      }

      template<typename = std::enable_if_t<std::is_copy_assignable<item_t>>>
      void value(const item_t& value)
      {
        *_value = value;
        on_changed();
      }

      template<typename = std::enable_if_t<std::is_move_assignable<item_t>>>
      void value(item_t&& value)
      {
        *_value = std::move(value);
        on_changed();
      }

      virtual void serialize(Streams::stream& stream) const override
      {
        stream.write(*_value);
      }

      virtual void deserialize(Streams::stream& stream) override
      {
        stream.read(*_value);
      }

      void apply_change(change* change)
      {
        switch (change->type())
        {
        case change_type::value_update:
        {
          auto valueUpdate = static_cast<value_update_change*>(change);
          valueUpdate->data.read(*_value);
        }
        break;
        default:
        {
          if constexpr (std::is_convertible<item_t*, observable*>::value)
          {
            _value->apply_change(change);
          }
          else
          {
            throw std::exception("This object does not support the specified change type!");
          }
        }
        break;
        }
      }
    };

    Events::event_owner _events;
    std::vector<item_container> _items;

    void on_added(uint32_t index)
    {
      auto change = std::make_unique<vector_item_insertion_change>();
      change->index = index;
      report_change(std::move(change));

      _events.raise(added, this, index);
    }

    void on_removed(uint32_t index)
    {
      auto change = std::make_unique<vector_item_removal_change>();
      change->index = index;
      report_change(std::move(change));

      _events.raise(removed, this, index);
    }

    void verify_index(uint32_t index)
    {
      if (index < 0 || index >= _items.size()) throw std::out_of_range("The specified item index is out of range.");
    }

    item_container new_item()
    {
      return item_container{ [&](item_container* source, std::unique_ptr<change>&& change) {
        auto index = (uint32_t)(source - _items.data());

        auto itemChange = std::make_unique<vector_item_update_change>();
        itemChange->index = index;
        itemChange->value = std::move(change);
        report_change(std::move(itemChange));

        _events.raise(changed, this, index);
        } };
    }

  public:
    observable_vector(const callback_t& callback) :
      observable(callback),
      added(_events),
      changed(_events),
      removed(_events)
    { }

    auto& push_back()
    {
      _items.push_back(new_item());
      on_added(uint32_t(_items.size() - 1));
      return _items.back().value();
    }

    auto& insert(uint32_t index)
    {
      if (index < 0 || index > _items.size()) throw std::out_of_range("The specified item index is out of range.");

      auto it = _items.insert(_items.begin() + index, new_item());
      on_added(index);
      return it->value();
    }

    auto& push_front()
    {
      _items.push_front(new_item());
      on_added(0);
      return _items.front().value();
    }

    void pop_back()
    {
      _items.pop_back();
      on_removed(_items.size());
    }

    void pop_front()
    {
      _items.pop_front();
      on_removed(0);
    }

    void erase(uint32_t index)
    {
      verify_index(index);
      _items.erase(_items.begin() + index);
      on_removed(index);
    }

    auto item(uint32_t index)
    {
      verify_index(index);
      return _items.at(index).value();
    }

    template<typename = std::enable_if_t<std::is_copy_assignable<item_t>>>
    void item(uint32_t index, const item_t& value)
    {
      verify_index(index);
      _items.at(index).value(value);
    }

    template<typename = std::enable_if_t<std::is_move_assignable<item_t>>>
    void item(uint32_t index, item_t&& value)
    {
      verify_index(index);
      _items.at(index).value(std::move(value));
    }

    size_t size() const
    {
      return _items.size();
    }

    virtual void apply_change(change* change) override
    {
      switch (change->type())
      {
      case change_type::value_update:
      {
        auto valueUpdate = static_cast<value_update_change*>(change);
        deserialize(valueUpdate->data);
      }
      break;
      case change_type::vector_item_insertion:
      {
        auto itemInsertion = static_cast<vector_item_insertion_change*>(change);
        insert(itemInsertion->index);
      }
      break;
      case change_type::vector_item_update:
      {
        auto itemUpdate = static_cast<vector_item_update_change*>(change);
        _items.at(itemUpdate->index).apply_change(itemUpdate->value.get());
      }
      break;
      case change_type::vector_item_removal:
      {
        auto item_removal = static_cast<vector_item_removal_change*>(change);
        erase(item_removal->index);
      }
      break;
      default:
        throw std::exception("This object does not support the specified change type!");
      }
    }

    virtual void serialize(Streams::stream& stream) const override
    {
      stream.write((uint32_t)_items.size());
      for (auto& item : _items)
      {
        stream.write(item);
      }
    }

    virtual void deserialize(Streams::stream& stream) override
    {
      auto length = stream.read<uint32_t>();
      _items.reserve(length);
      for (auto i = 0u; i < length; i++)
      {
        auto item = new_item();
        stream.read(item);
        _items.push_back(std::move(item));
      }
    }
  };

  class observable_property_base
  {
  public:
    virtual uint16_t key() const = 0;
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
  };

  template <typename TPropertyKey, typename = std::enable_if_t<std::is_same<std::underlying_type_t<TPropertyKey>, uint16_t>::value>>
  class observable_object : public observable
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

    }
  };
}