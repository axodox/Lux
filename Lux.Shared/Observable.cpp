#include "pch.h"
#include "Observable.h"
#include "ObservableValue.h"
#include "ObservableObject.h"
#include "ObservableVector.h"

namespace Lux::Observable
{
  Serialization::type_registry<change> change::actual_types = Serialization::make_type_registry<change,
    value_update_change,
    object_property_update_change,
    vector_item_insertion_change,
    vector_item_update_change,
    vector_item_removal_change>();
  
  void observable::report_change(std::unique_ptr<change>&& change)
  {
    _callback(std::move(change));
  }
  
  observable::observable(const callback_t& callback) :
    _callback(callback)
  { }
}