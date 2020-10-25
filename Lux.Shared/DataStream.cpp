#include "pch.h"
#include "DataStream.h"

using namespace std;
using namespace std::chrono;
using namespace std::chrono_literals;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Storage::Streams;

namespace Lux::Serialization
{
  data_stream::data_stream(IDataWriter writer) :
    _writer(writer),
    _reader(nullptr)
  { }

  data_stream::data_stream(IDataReader reader) :
    _writer(nullptr),
    _reader(reader)
  { }

  void data_stream::write(size_t length, const uint8_t* bytes)
  {
    if (_writer == nullptr) throw exception("Cannot write to a DataReader.");

    _writer.WriteBytes({ bytes, bytes + length });
  }

  void data_stream::read(size_t length, uint8_t* bytes)
  {
    if (_reader == nullptr) throw exception("Cannot read from a DataWriter.");

    auto loadedLength = _reader.UnconsumedBufferLength();
    
    steady_clock::time_point time{};
    while (length > loadedLength)
    {
      auto task = _reader.LoadAsync((uint32_t)length - loadedLength);
      if (timeout != 0ms)
      {
        auto now = steady_clock::now();
        if (time == steady_clock::time_point{})
        {
          time = now;
        }
        else
        {
          if (now - time > timeout)
          {
            throw exception("Data stream read timed out!");
          }
        }

        switch (task.wait_for({ timeout }))
        {
        case AsyncStatus::Completed:
          loadedLength += task.GetResults();
          break;
        case AsyncStatus::Started:
          throw exception("Data stream read timed out!");
          break;
        default:
          throw exception("Data stream read failed!");
          break;
        }
      }
      else
      {
        loadedLength += _reader.LoadAsync((uint32_t)length - loadedLength).get();
        this_thread::sleep_for(10ms);
      }      
    }

    _reader.ReadBytes({ bytes, bytes + length });
  }

  size_t data_stream::position() const
  {
    throw exception("This method is not supported.");
  }

  void data_stream::seek(size_t /*position*/)
  {
    throw exception("This method is not supported.");
  }

  size_t data_stream::length() const
  {
    throw exception("This method is not supported.");
  }

  void data_stream::flush()
  {
    if (_writer == nullptr) throw exception("Cannot flush a DataReader.");

    _writer.StoreAsync().get();
  }

  buffered_data_stream::buffered_data_stream(IDataWriter writer) :
    data_stream(writer),
    _buffer({})
  {

  }

  buffered_data_stream::buffered_data_stream(IDataReader reader) :
    data_stream(reader),
    _buffer({})
  {
    _buffer.resize(_reader.ReadUInt32());
    _reader.ReadBytes(static_cast<vector<uint8_t>&>(_buffer));
  }

  void buffered_data_stream::write(size_t length, const uint8_t* bytes)
  {
    _buffer.write(length, bytes);
  }

  void buffered_data_stream::read(size_t length, uint8_t* bytes)
  {
    _buffer.read(length, bytes);
  }

  buffered_data_stream::~buffered_data_stream()
  {
    if (_writer != nullptr)
    {
      _writer.WriteUInt32((uint32_t)_buffer.size());
      _writer.WriteBytes(static_cast<const vector<uint8_t>&>(_buffer));
    }
  }
}
