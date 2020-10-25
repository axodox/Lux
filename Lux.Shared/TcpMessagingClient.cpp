#include "pch.h"
#include "TcpMessagingClient.h"
#include "ResetEvent.h"

using namespace Lux::Events;
using namespace Lux::Threading;
using namespace std;
using namespace winrt;
using namespace winrt::Windows::Networking;
using namespace winrt::Windows::Networking::Sockets;

namespace Lux::Networking
{
  tcp_messaging_client::tcp_messaging_client(ip_endpoint const& endpoint) :
    _endpoint(endpoint),
    _connection_thread([&] { connect(); }),
    connected(_events),
    is_connected_changed(_events)
  { }

  tcp_messaging_client::~tcp_messaging_client()
  {
    _is_disposed.set();
  }

  message_task tcp_messaging_client::send(Serialization::memory_stream&& message)
  {
    lock_guard<mutex> lock(_mutex);
    return _channel->send(move(message));
  }

  bool tcp_messaging_client::is_connected() const
  {
    return _channel != nullptr;
  }

  void tcp_messaging_client::connect()
  {
    while (!_is_disposed.wait(0ms))
    {
      try
      {
        StreamSocket socket;
        socket.ConnectAsync(HostName{ _endpoint.hostname }, to_hstring(_endpoint.port)).get();

        manual_reset_event event;
        {
          lock_guard<mutex> lock(_mutex);
          _channel = unique_ptr<tcp_messaging_channel>{ new tcp_messaging_channel(socket) };
          _events.raise(connected, this, _channel.get());
          _events.raise(is_connected_changed, this, true);

          _channel->disconnected(no_revoke, [&](tcp_messaging_channel*) { event.set(); });
          _channel->open();
        }

        reset_event_base::wait_any({ &event, &_is_disposed });
      }
      catch (...)
      {
        //log
      }

      {
        lock_guard<mutex> lock(_mutex);
        _channel.reset();
        _events.raise(is_connected_changed, this, false);
      }
    }
  }
}