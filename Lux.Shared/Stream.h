#pragma once
#include "shared_pch.h"

namespace Lux::Serialization
{
  class stream
  {
  public:
    virtual void write(size_t length, const uint8_t* bytes) = 0;
    virtual void read(size_t length, uint8_t* bytes) = 0;

    virtual size_t position() const = 0;
    virtual void seek(size_t position) = 0;
    virtual size_t length() const = 0;
    virtual ~stream() = default;

    template<typename T>
    void write(const T& value);

    template<typename T>
    void read(T& value);

    template<typename T>
    T read();
  };

  template<typename T, typename TEnable = void>
  struct serializer { };

  template<typename T>
  void stream::write(const T& value)
  {
    serializer<T>::serialize(*this, value);
  }

  template<typename T>
  void stream::read(T& value)
  {
    serializer<T>::deserialize(*this, value);
  }

  template<typename T>
  T stream::read()
  {
    T value;
    serializer<T>::deserialize(*this, value);
    return value;
  }
}