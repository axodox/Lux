#include "pch.h"
#include "TcpMessagingServer.h"
#include "AwaitOperation.h"

using namespace Lux::Serialization;
using namespace std;
using namespace winrt;
using namespace winrt::Windows::Networking::Sockets;

namespace Lux::Networking
{
  tcp_messaging_server::tcp_messaging_server(uint16_t port) :
    client_connected(_events),
    client_disconnected(_events)
  {
    _connection_received_revoker = _socket.ConnectionReceived(auto_revoke, { this, &tcp_messaging_server::on_connection_received });
    _socket.BindServiceNameAsync(to_hstring(port)).get();
  }

  tcp_messaging_server::~tcp_messaging_server()
  {
    _socket.Close();
  }

  void tcp_messaging_server::broadcast(Serialization::memory_stream&& message)
  {
    shared_lock<shared_mutex> lock(_mutex);

    if (_channels.size() == 1)
    {
      _channels.begin()->second->send(move(message));
    }
    else
    {
      for (auto& [key, channel] : _channels)
      {
        memory_stream copy{ message };
        channel->send(move(copy));
      }
    }
  }

  void tcp_messaging_server::on_connection_received(const StreamSocketListener&, const StreamSocketListenerConnectionReceivedEventArgs& eventArgs)
  {
    unique_ptr<tcp_messaging_channel> client{ new tcp_messaging_channel(eventArgs.Socket()) };
    _events.raise(client_connected, this, client.get());

    client->disconnected(Events::no_revoke, [&](tcp_messaging_channel* channel) {
      unique_lock<shared_mutex> lock(_mutex);
      _channels.erase(channel);
      });
    client->open();

    unique_lock<shared_mutex> lock(_mutex);
    _channels.emplace(client.get(), move(client));
  }
}