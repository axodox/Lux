#pragma once
#include "TcpMessagingChannel.h"

namespace Lux::Networking
{
  class tcp_messaging_server final
  {
  public:
    tcp_messaging_server(uint16_t port);
    ~tcp_messaging_server();

    Events::event_publisher<tcp_messaging_server*, tcp_messaging_channel*> client_connected, client_disconnected;

    void broadcast(Serialization::memory_stream&& message);

  private:
    Events::event_owner _events;
    winrt::Windows::Networking::Sockets::StreamSocketListener _socket;

    std::shared_mutex _mutex;
    std::unordered_map<tcp_messaging_channel*, std::unique_ptr<tcp_messaging_channel>> _channels;

    winrt::Windows::Networking::Sockets::StreamSocketListener::ConnectionReceived_revoker _connection_received_revoker;

    void on_connection_received(const winrt::Windows::Networking::Sockets::StreamSocketListener& sender, const winrt::Windows::Networking::Sockets::StreamSocketListenerConnectionReceivedEventArgs& eventArgs);
  };
}