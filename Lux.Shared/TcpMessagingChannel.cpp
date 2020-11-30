#include "pch.h"
#include "TcpMessagingChannel.h"
#include "DataStream.h"

using namespace Lux::Serialization;
using namespace Lux::Threading;
using namespace std;
using namespace winrt;
using namespace winrt::Windows::Networking::Sockets;
using namespace winrt::Windows::Storage::Streams;

namespace Lux::Networking
{
  tcp_messaging_channel::tcp_messaging_channel(const StreamSocket& socket) :
    disconnected(_events)
  {
    _socket = socket;
  }

  message_task tcp_messaging_channel::send(Serialization::memory_stream&& message)
  {
    auto messagePromise = make_shared<message_promise>(move(message));
    message_task task{ messagePromise };
    _messages_to_send.add(std::move(messagePromise));
    return task;
  }

  tcp_messaging_channel::~tcp_messaging_channel()
  {
    _socket.Close();
    on_disconnected();
  }

  void tcp_messaging_channel::on_opening()
  {
    _send_thread = make_unique<background_thread>([&] { send(); }, L"TCP sender thread");
    _receive_thread = make_unique<background_thread>([&] { receive(); }, L"TCP receiver thread");
  }

  void tcp_messaging_channel::send()
  {
    try
    {
      DataWriter writer{ _socket.OutputStream() };
      data_stream stream{ writer };

      std::shared_ptr<message_promise> task{};
      while (is_connected())
      {
        if (_messages_to_send.try_get(task, 200ms))
        {
          stream.write(_magic);
          stream.write(task->message());
          task->finish(true);
          task = nullptr;
        }
        else //heartbeat
        {
          stream.write(_magic);
          stream.write(memory_stream{});
        }
        stream.flush();
      }
    }
    catch (...)
    {
      //log
    }

    _messages_to_send.complete();
    _messages_to_send.clear();

    on_disconnected();
  }

  void tcp_messaging_channel::receive()
  {
    try
    {
      DataReader reader{ _socket.InputStream() };
      data_stream stream{ reader };
      stream.timeout = 1000ms;

      while (is_connected())
      {
        auto magic = stream.read<uint64_t>();
        if (magic != _magic)
        {
          //log
          throw exception("Magic does not match!");
        }

        memory_stream message;
        stream.read(message);

        if (message.size() > 0)
        {
          on_received(move(message));
        }        
      }
    }
    catch(...)
    {
      //log
    }

    on_disconnected();
  }
}