#include "pch.h"
#include "FileStream.h"

using namespace std;
using namespace std::filesystem;

namespace Lux::Serialization
{
  void file_stream::check_mode(file_mode mode) const
  {
    if (((uint8_t)_mode & (uint8_t)mode) == 0u)
    {
      throw exception("Attempted invalid operation on file stream!");
    }
  }

  file_stream::file_stream(const filesystem::path& path, file_mode mode) :
    _path(path),
    _mode(mode)
  {
    wstring fileMode;
    switch (mode)
    {
    case file_mode::read:
      fileMode = L"rb";
      break;
    case file_mode::write:
      fileMode = L"wb";
      break;
    case file_mode::read_write:
      fileMode = L"wb+";
      break;
    default:
      throw exception("Invalid file mode specified!");
    }
    if (_wfopen_s(&_file, path.c_str(), fileMode.c_str()) != 0)
    {
      throw exception("Failed to open file!");
    }
  }

  file_stream::file_stream(file_stream&& other)
  {
    _file = other._file;
    other._file = nullptr;
    _mode = other._mode;
    other._mode = file_mode::none;
    _path = other._path;
    other._path = L"";
  }

  const filesystem::path& file_stream::path() const
  {
    return _path;
  }

  file_stream::~file_stream()
  {
    if (_file)
    {
      fclose(_file);
      _file = nullptr;
    }
  }

  void file_stream::write(size_t length, const uint8_t* bytes)
  {
    check_mode(file_mode::write);
    fwrite(bytes, 1, length, _file);
  }

  void file_stream::read(size_t length, uint8_t* bytes)
  {
    check_mode(file_mode::read);
    fread(bytes, 1, length, _file);
  }

  size_t file_stream::position() const
  {
    check_mode(file_mode::read_write);
    return ftell(_file);
  }

  void file_stream::seek(size_t position)
  {
    check_mode(file_mode::read_write);
    fseek(_file, (long)position, SEEK_SET);
  }

  size_t file_stream::length() const
  {
    check_mode(file_mode::read_write);

    auto pos = position();
    fseek(_file, 0, SEEK_END);
    auto length = position();
    fseek(_file, (long)pos, SEEK_SET);
    return length;
  }

  void file_stream::flush()
  {
    fflush(_file);
  }
}
