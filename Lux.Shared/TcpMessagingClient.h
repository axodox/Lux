#pragma once
#include "TcpMessagingChannel.h"

namespace Lux::Networking
{
  struct ip_endpoint
  {
    std::wstring hostname;
    uint16_t port;
  };

  class tcp_messaging_client final
  {
  public:
    tcp_messaging_client(ip_endpoint const& endpoint);
    Events::event_publisher<tcp_messaging_client*, tcp_messaging_channel*> connected;
    
    message_task send(Serialization::memory_stream&& message);

    bool is_connected() const;
    Events::event_publisher<tcp_messaging_client*, bool> is_connected_changed;

    ~tcp_messaging_client();

  private:
    Events::event_owner _events;
    ip_endpoint _endpoint;
    std::mutex _mutex;
    std::unique_ptr<tcp_messaging_channel> _channel;
    Threading::background_thread _connection_thread;
    Threading::manual_reset_event _is_disposed;

    void connect();
  };
}
