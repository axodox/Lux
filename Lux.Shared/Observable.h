#pragma once
#include "shared_pch.h"
#include "TypeRegistry.h"
#include "Serializer.h"

namespace Lux::Observable
{
  enum class change_type : Serialization::type_id_t
  {
    unknown,
    value_update,
    object_property_update,
    vector_item_insertion,
    vector_item_update,
    vector_item_removal
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
}