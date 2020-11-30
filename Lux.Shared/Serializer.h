#pragma once
#include "Stream.h"
#include "Traits.h"
#include "TypeRegistry.h"

namespace Lux::Serialization
{
  template<typename T>
  struct serializer<T, typename std::enable_if_t<std::is_trivially_copyable<T>::value>>
  {
    static void serialize(stream& stream, const T& value)
    {
      stream.write(sizeof(T), reinterpret_cast<const uint8_t*>(&value));
    }

    static void deserialize(stream& stream, T& value)
    {
      stream.read(sizeof(T), reinterpret_cast<uint8_t*>(&value));
    }
  };

  template<typename T>
  struct serializer<typename T, typename std::enable_if_t<std::is_same<T, std::wstring>::value>>
  {
    static void serialize(stream& stream, const std::wstring& value)
    {
      stream.write((uint32_t)value.size());
      stream.write(
        value.size() * sizeof(std::wstring::value_type),
        reinterpret_cast<const uint8_t*>(value.data()));
    }

    static void deserialize(stream& stream, std::wstring& value)
    {
      value.resize(stream.read<uint32_t>());
      stream.read(
        value.size() * sizeof(std::wstring::value_type),
        reinterpret_cast<uint8_t*>(value.data()));
    }
  };

  template<typename T>
  struct serializer<typename T, typename std::enable_if_t<Traits::is_instantiation_of<std::vector, T>::value>>
  {
    static void serialize(stream& stream, const T& value)
    {
      stream.write((uint32_t)value.size());

      for (auto& item : value)
      {
        stream.write(item);
      }
    }

    static void deserialize(stream& stream, T& value)
    {
      value.resize(stream.read<uint32_t>());

      for (auto& item : value)
      {
        stream.read(item);
      }
    }
  };

  struct serializable
  {
    virtual void serialize(stream& stream) const = 0;
    virtual void deserialize(stream& stream) = 0;

    virtual ~serializable() = default;
  };

  template<typename T>
  struct serializer<typename T, typename std::enable_if_t<std::is_convertible<T*, serializable*>::value>>
  {
    static void serialize(stream& stream, const T& value)
    {
      value.serialize(stream);
    }

    static void deserialize(stream& stream, T& value)
    {
      value.deserialize(stream);
    }
  };

  template<typename T>
  struct serializer<typename T, typename std::enable_if_t<std::conjunction<
    Traits::is_instantiation_of<std::unique_ptr, T>, 
    Traits::has_actual_types<typename T::element_type>>::value>>
  {
    static const type_id_t empty_type = (type_id_t)-1;

    static void serialize(stream& stream, const T& value)
    {
      if (value)
      {
        stream.write((type_id_t)value->type());
        stream.write(*value);
      }
      else
      {
        stream.write(empty_type);
      }
    }

    static void deserialize(stream& stream, T& value)
    {
      auto typeId = stream.read<type_id_t>();
      if (typeId != empty_type)
      {
        value = T::element_type::actual_types.create_unique(typeId);
        stream.read(*value);
      }
      else
      {
        value = {};
      }
    }
  };
}