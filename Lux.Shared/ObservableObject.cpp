#include "pch.h"
#include "ObservableObject.h"

namespace Lux::Observable
{
  change_type object_property_update_change::type() const
  {
    return change_type::object_property_update;
  }
  
  void object_property_update_change::serialize(Serialization::stream& stream) const
  {
    stream.write(key);
    stream.write(value);
  }
  
  void object_property_update_change::deserialize(Serialization::stream& stream)
  {
    stream.read(key);
    stream.read(value);
  }
}