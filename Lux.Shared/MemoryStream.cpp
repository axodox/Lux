#include "pch.h"
#include "MemoryStream.h"

using namespace std;

namespace Lux::Serialization
{
  uint8_t* memory_stream::current()
  {
    return data() + _position;
  }

  void memory_stream::write(size_t length, const uint8_t* bytes)
  {
    auto newSize = _position + length;
    if (newSize > size())
    {
      resize(newSize);
    }
    memcpy(current(), bytes, length);
    _position += length;
  }

  void memory_stream::read(size_t length, uint8_t* bytes)
  {
    if (_position + length > size())
    {
      throw exception("Cannot read past the end of the stream!");
    }
    memcpy(bytes, current(), length);
    _position += length;
  }

  size_t memory_stream::position() const
  {
    return _position;
  }

  void memory_stream::seek(size_t position)
  {
    if (_position > size()) throw exception("Cannot seek past the end of the stream!");
    _position = position;
  }

  size_t memory_stream::length() const
  {
    return size();
  }

  memory_stream::operator std::vector<uint8_t>& ()
  {
    return *this;
  }

  memory_stream::operator winrt::Windows::Storage::Streams::Buffer() const
  {
    winrt::Windows::Storage::Streams::Buffer buffer((uint32_t)size());
    buffer.Length((uint32_t)size());
    memcpy(buffer.data(), data(), size());
    return buffer;
  }

  memory_stream::operator winrt::array_view<uint8_t>()
  {
    return { data(), data() + size() };
  }
}
