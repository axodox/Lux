#pragma once
#include "MessagingChannel.h"

namespace Lux::Networking
{
  class messaging_server
  {
  public:
    messaging_server();

    Events::event_publisher<messaging_server*, messaging_channel*> client_connected, client_disconnected;

    uint32_t client_count() const;

    void broadcast(Serialization::memory_stream&& message);

    virtual ~messaging_server() = default;

  protected:
    void on_client_connected(std::unique_ptr<messaging_channel>&& channel);

  private:
    Events::event_owner _events;
    std::shared_mutex _mutex;
    std::unordered_map<messaging_channel*, std::unique_ptr<messaging_channel>> _clients;    
  };
}