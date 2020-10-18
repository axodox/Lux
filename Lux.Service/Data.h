#pragma once
#include "Events.h"
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

  struct child_property_change_info
  {

  };

  struct binary_view
  {
    uint8_t* data = nullptr;
    size_t lenght = 0u;
  };

  class binary_buffer : std::vector<uint8_t>
  {

  };

  class observable
  {
  public:
    typedef std::function<void(std::unique_ptr<change>&&)> callback_t;

  private:
    callback_t _callback;

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
    Events::event_owner _events;
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
    { }

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
  class observable_list : public observable
  {
  public:
    typedef TItem item_t;

  private:
    class item_container
    {
      std::unique_ptr<item_t> _value;
      std::function<void(item_container*, std::unique_ptr<change>&&)> _callback;

      void on_chaged()
      {
        auto change = std::make_unique<value_update_change>();
        change->data.write(*_value);
        _callback(this, change);
      }

    public:
      item_container(const std::function<void(item_container*, std::unique_ptr<change>&&)>& callback) : 
        _callback(callback)
      {
        if constexpr (std::is_convertible<item_t*, observable*>::value)
        {
          _value = std::make_unique<item_t>([&](std::unique_ptr<change>&& change) {
            _callback(this, std::move(change));
            });
        }
        else
        {
          _value = std::make_unique<item_t>();
        }
      }

      const item_t& value() const
      {
        return *_value;
      }

      template<typename std::enable_if_t<std::is_copy_assignable<item_t>> = nullptr>
      void value(const item_t& value)
      {
        *_value = value;
        on_changed();
      }

      template<typename std::enable_if_t<std::is_move_assignable<item_t>> = nullptr>
      void value(item_t&& value)
      {
        *_value = std::move(value);
        on_changed();
      }
    };

    Events::event_owner _events;
    std::vector<item_container> _items;

    void on_added(uint32_t index)
    {
      auto change = std::make_unique<vector_item_insertion_change>();
      change->index = index;
      report_change(std::move(change));

      _events(added, this, index);
    }

    void on_changed(uint32_t index)
    {
      auto change = std::make_unique<vector_item_update_change>();
      change->index = index;

      auto value = std::make_unique<value_update_change>();
      value->data.write(*(_items.begin() + index));
      change->value = std::move(value);
      report_change(std::move(change));

      _events.raise(added, this, index);
    }

    void on_removed(uint32_t index)
    {
      auto change = std::make_unique<vector_item_removal_change>();
      change->index = index;
      report_change(std::move(change));

      _events(added, this, index);
    }

    void verify_index(uint32_t index)
    {
      if (index < 0 || index >= _items.size()) throw std::out_of_range("The specified item index is out of range.");
    }

    item_t new_item(typename std::list<item_t>::iterator*& targetIt)
    {
      if constexpr (std::is_convertible<item_t, observable>::value)
      {
        auto itContainer = std::make_unique<std::list<item_t>::iterator>();
        targetIt = itContainer.get();
        return item_t([&, it = std::move(itContainer)](std::unique_ptr<change>&& change) {
          auto itemChange = std::make_unique<vector_item_update_change>();
          itemChange->index = *it - _items.begin();
          itemChange->value = std::move(change);
          _callback(std::make_unique<vector_item_update_change>(itemChange));
          });
      }
      else
      {
        return item_t{};
      }
    }

  public:
    Events::event_publisher<observable_list<item_t>*, uint32_t> added, changed, removed;

    observable_list(const callback_t& callback) :
      observable(callback),
      added(_events),
      changed(_events),
      removed(_events)
    { }

    auto begin() const
    {
      return _items.begin();
    }

    auto end() const
    {
      return _items.end();
    }

    item_t& push_back()
    {
      typename std::list<item_t>::iterator* targetIt;
      _items.push_back(new_item(targetIt));
      *targetIt = _items.end() - 1;
      on_added(_items.size() - 1);
      return _items.back();
    }

    item_t& insert(uint32_t index)
    {
      if(index < 0 || index > _items.size()) throw std::out_of_range("The specified item index is out of range.");

      typename std::list<item_t>::iterator* targetIt;
      auto it = _items.insert(_items.begin() + index, new_item(targetIt));
      *targetIt = it;
      on_added(index);
      return it;
    }

    item_t& push_front()
    {
      typename std::list<item_t>::iterator* targetIt;
      _items.push_front(new_item(targetIt));
      *targetIt = _items.begin();
      on_added(0);
      return _items.front();
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

    item_t& item(uint32_t index) const
    {
      verify_index(index);
      return *(_items.begin() + index);
    }

    void item(uint32_t index, const item_t& value)
    {
      verify_index(index);
      *(_items.begin() + index) = value;
      on_changed(index);
    }

    void item(uint32_t index, item_t&& value)
    {
      verify_index(index);
      *(_items.begin() + index) = std::move(value);
      on_changed(index);
    }

    virtual void apply_change(change* change) override
    {
      switch (change->type())
      {
      case change_type::value_update:
      {
        auto valueUpdate = static_cast<value_update_change*>(change);
        valueUpdate->data.read(_items);
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
        if (itemUpdate->value->type() == change_type::value_update)
        {
          item(itemUpdate->index, static_cast<value_update_change*>(itemUpdate->value.get())->data.read<item_t>());
        }
        else if constexpr (std::is_convertible<item_t, observable>::value)
        {
          item(itemUpdate->index).apply_change(itemUpdate->value);
        }
        else
        {
          throw std::exception("This object does not support the specified change type!");
        }
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
  };



  class observable_object;

  class observable_property_base
  {
    virtual uint16_t key() = 0;
  };

  template<typename TValue>
  class observable_property : public observable_value<TValue>
  {
  public:
    const uint16_t key;

    observable_property(observable_object* owner, uint16_t key, const TValue& value = {}) :
      observable_value(value),
      key(key)
    { }
  };

  class observable_object
  {

  };
}