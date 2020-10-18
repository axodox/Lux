#pragma once
#include "Stream.h"

namespace Lux::Streams
{
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

  struct serializable
  {
    virtual void serialize(stream& stream) const = 0;
    virtual void deserialize(stream& stream) = 0;

    virtual ~serializable() = default;
  };

  template<typename T>
  struct serializer<typename T, typename std::enable_if_t<std::is_convertible<T*, Streams::serializable*>::value>>
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
}