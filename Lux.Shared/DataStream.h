#pragma once
#include "Stream.h"
#include "MemoryStream.h"

namespace Lux::Serialization
{
  class data_stream : public stream
  {
  protected:
    winrt::Windows::Storage::Streams::IDataWriter _writer;
    winrt::Windows::Storage::Streams::IDataReader _reader;

  public:
    std::chrono::milliseconds timeout = {};

    using stream::write;
    using stream::read;

    data_stream(winrt::Windows::Storage::Streams::IDataWriter writer);
    data_stream(winrt::Windows::Storage::Streams::IDataReader reader);

    virtual void write(size_t length, const uint8_t* bytes) override;
    virtual void read(size_t length, uint8_t* bytes) override;

    virtual size_t position() const override;
    virtual void seek(size_t position) override;
    virtual size_t length() const override;

    void flush();
  };

  class buffered_data_stream : public data_stream
  {
  private:
    memory_stream _buffer;

  public:
    using data_stream::write;
    using data_stream::read;

    buffered_data_stream(winrt::Windows::Storage::Streams::IDataWriter writer);
    buffered_data_stream(winrt::Windows::Storage::Streams::IDataReader reader);

    virtual void write(size_t length, const uint8_t* bytes) override;
    virtual void read(size_t length, uint8_t* bytes) override;

    virtual ~buffered_data_stream();
  };
}