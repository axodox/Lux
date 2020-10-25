#include "pch.h"
#include "ObservableValue.h"

namespace Lux::Observable
{
  change_type value_update_change::type() const
  {
    return change_type::value_update;
  }
  
  void value_update_change::serialize(Serialization::stream& stream) const
  {
    stream.write(data);
  }
  
  void value_update_change::deserialize(Serialization::stream& stream)
  {
    stream.read(data);
  }
}