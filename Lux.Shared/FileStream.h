#pragma once
#include "Stream.h"

namespace Lux::Serialization
{
  enum class file_mode : uint8_t
  {
    none = 0,
    read = 1,
    write = 2,
    read_write = read | write
  };

  class file_stream : public stream
  {
  private:
    std::filesystem::path _path;
    file_mode _mode;
    FILE* _file;

    void check_mode(file_mode mode) const;

  public:
    using stream::write;
    using stream::read;

    file_stream(const std::filesystem::path& path, file_mode mode);

    file_stream(const file_stream&) = delete;
    file_stream(file_stream&& other);

    const std::filesystem::path& path() const;

    ~file_stream();

    virtual void write(size_t length, const uint8_t* bytes) override;

    virtual void read(size_t length, uint8_t* bytes) override;

    virtual size_t position() const override;

    virtual void seek(size_t position) override;

    virtual size_t length() const override;

    void flush();
  };
}