#pragma once
#include "MessagingServer.h"

namespace Lux::Networking
{
  class tcp_messaging_server final : public messaging_server
  {
  public:
    tcp_messaging_server(uint16_t port);
    ~tcp_messaging_server();

  private:
    winrt::Windows::Networking::Sockets::StreamSocketListener _socket;
    winrt::Windows::Networking::Sockets::StreamSocketListener::ConnectionReceived_revoker _connection_received_revoker;

    void on_connection_received(const winrt::Windows::Networking::Sockets::StreamSocketListener& sender, const winrt::Windows::Networking::Sockets::StreamSocketListenerConnectionReceivedEventArgs& eventArgs);
  };
}