#pragma once
#include "Observable.h"
#include "Serializer.h"
#include "ObservableValue.h"

namespace Lux::Observable
{
  struct vector_item_change : public change
  {
    uint32_t index;

    virtual void serialize(Serialization::stream& stream) const override;
    virtual void deserialize(Serialization::stream& stream) override;
  };

  struct vector_item_insertion_change : public vector_item_change
  {
    virtual change_type type() const;
  };

  struct vector_item_update_change : public vector_item_change
  {
    std::unique_ptr<change> value;

    virtual change_type type() const;

    virtual void serialize(Serialization::stream& stream) const override;
    virtual void deserialize(Serialization::stream& stream) override;
  };

  struct vector_item_removal_change : public vector_item_change
  {
    virtual change_type type() const;
  };

  template<typename TItem>
  class observable_vector : public observable, public Serialization::serializable
  {
  public:
    typedef TItem item_t;
    Events::event_publisher<observable_vector<item_t>*, uint32_t> added, changed, removed;

  private:
    class item_container : public Serialization::serializable
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

      virtual void serialize(Serialization::stream& stream) const override
      {
        stream.write(*_value);
      }

      virtual void deserialize(Serialization::stream& stream) override
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

    virtual void serialize(Serialization::stream& stream) const override
    {
      stream.write((uint32_t)_items.size());
      for (auto& item : _items)
      {
        stream.write(item);
      }
    }

    virtual void deserialize(Serialization::stream& stream) override
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
}