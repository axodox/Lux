#include "pch.h"
#include "ObservableVector.h"

namespace Lux::Observable
{
  void vector_item_change::serialize(Serialization::stream& stream) const
  {
    stream.write(index);
  }
  
  void vector_item_change::deserialize(Serialization::stream& stream)
  {
    stream.read(index);
  }
  
  change_type vector_item_insertion_change::type() const
  {
    return change_type::vector_item_insertion;
  }
  
  change_type vector_item_update_change::type() const
  {
    return change_type::vector_item_update;
  }
  
  void vector_item_update_change::serialize(Serialization::stream& stream) const
  {
    vector_item_change::serialize(stream);
    stream.write(value);
  }
  
  void vector_item_update_change::deserialize(Serialization::stream& stream)
  {
    vector_item_change::deserialize(stream);
    stream.read(value);
  }
  
  change_type vector_item_removal_change::type() const
  {
    return change_type::vector_item_removal;
  }
}