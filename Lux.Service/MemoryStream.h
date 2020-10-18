#pragma once
#include "Stream.h"

namespace Lux::Streams
{
  class memory_stream : public std::vector<uint8_t>, public stream
  {
  private:
    size_t _position = 0u;
    uint8_t* current();

  public:
    using vector::vector;

    using stream::write;
    using stream::read;

    virtual void write(size_t length, const uint8_t* bytes) override;
    virtual void read(size_t length, uint8_t* bytes) override;

    virtual size_t position() const override;
    virtual void seek(size_t position) override;
    virtual size_t length() const override;

    operator winrt::array_view<uint8_t>();
    operator std::vector<uint8_t>& ();
    operator winrt::Windows::Storage::Streams::Buffer() const;
  };
}