#pragma once
#include "shared_pch.h"
#include "TypeRegistry.h"
#include "Serializer.h"
#include "Events.h"

namespace Lux::Observable
{
  enum class change_type : Serialization::type_id_t
  {
    unknown,
    value_update,
    object_property_update,
  };

  struct change : public Serialization::serializable
  {
    static Serialization::type_registry<change> actual_types;

    virtual change_type type() const = 0;

    virtual ~change() = default;
  };

  class observable
  {
  public:
    typedef std::function<void(std::unique_ptr<change>&&)> callback_t;

  private:
    const callback_t _callback;

  protected:
    void report_change(std::unique_ptr<change>&& change);

  public:
    observable(const callback_t& callback);

    virtual void apply_change(change* change) = 0;
  };

  template<typename T, typename = std::enable_if_t<std::is_convertible<T*, observable*>::value>>
  class observable_root : public T
  {
  private:
    Events::event_owner _events;
    
    void on_change(std::unique_ptr<change>&& change)
    {
      _events.raise(change_reported, this, std::move(change));
    }

  public:
    observable_root() :
      T([&](std::unique_ptr<change>&& change) { on_change(std::move(change)); }),
      change_reported(_events)
    { }

    Events::event_publisher<observable_root<T>*, std::unique_ptr<change>&&> change_reported;
  };
}