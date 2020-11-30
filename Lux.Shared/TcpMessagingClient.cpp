#include "pch.h"
#include "TcpMessagingClient.h"
#include "TcpMessagingChannel.h"
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
    _endpoint(endpoint)
  { }

  ip_endpoint tcp_messaging_client::endpoint() const
  {
    return _endpoint;
  }

  std::unique_ptr<messaging_channel> tcp_messaging_client::get_client()
  {
    StreamSocket socket;
    socket.ConnectAsync(HostName{ _endpoint.hostname }, to_hstring(_endpoint.port)).get();
    return unique_ptr<tcp_messaging_channel>{ new tcp_messaging_channel(socket) };
  }
}