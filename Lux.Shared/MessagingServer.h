#pragma once
#include "MessagingChannel.h"
#include "Openable.h"

namespace Lux::Networking
{
  class messaging_server : public Infrastructure::openable
  {
  private:
    Events::event_owner _events;

  public:
    messaging_server();

    Events::event_publisher<messaging_server*, messaging_channel*> client_connected, client_disconnected;

    uint32_t client_count() const;

    void broadcast(Serialization::memory_stream&& message, messaging_channel* exception = nullptr);

  protected:
    void on_client_connected(std::unique_ptr<messaging_channel>&& channel);

  private:
    std::shared_mutex _mutex;
    std::unordered_map<messaging_channel*, std::unique_ptr<messaging_channel>> _clients;
    std::unique_ptr<messaging_channel> _client_parking_space;
  };
}