#include "pch.h"
#include "TcpMessagingServer.h"
#include "TcpMessagingChannel.h"
#include "AwaitOperation.h"

using namespace Lux::Serialization;
using namespace std;
using namespace winrt;
using namespace winrt::Windows::Networking::Sockets;

namespace Lux::Networking
{
  tcp_messaging_server::tcp_messaging_server(uint16_t port)
  {
    _connection_received_revoker = _socket.ConnectionReceived(auto_revoke, { this, &tcp_messaging_server::on_connection_received });
    _socket.BindServiceNameAsync(to_hstring(port)).get();
  }

  tcp_messaging_server::~tcp_messaging_server()
  {
    _socket.Close();
  }

  void tcp_messaging_server::on_connection_received(const StreamSocketListener&, const StreamSocketListenerConnectionReceivedEventArgs& eventArgs)
  {
    on_client_connected(unique_ptr<messaging_channel>{ new tcp_messaging_channel(eventArgs.Socket()) });
  }
}