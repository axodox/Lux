#include "pch.h"
#include "MessagingServer.h"

using namespace Lux::Serialization;
using namespace std;

namespace Lux::Networking
{
  messaging_server::messaging_server() :
    client_connected(_events),
    client_disconnected(_events)
  { }

  uint32_t messaging_server::client_count() const
  {
    return uint32_t(_clients.size());
  }

  void messaging_server::broadcast(Serialization::memory_stream && message, messaging_channel* exception)
  {
    shared_lock<shared_mutex> lock(_mutex);

    if (_clients.size() == 1)
    {
      _clients.begin()->second->send(move(message));
    }
    else
    {
      for (auto& [key, client] : _clients)
      {
        if (exception == client.get()) continue;

        memory_stream copy{ message };
        client->send(move(copy));
      }
    }
  }

  void messaging_server::on_client_connected(std::unique_ptr<messaging_channel>&& channel)
  {
    _events.raise(client_connected, this, channel.get());

    channel->disconnected(Events::no_revoke, [&](messaging_channel* channel) {
      unique_lock<shared_mutex> lock(_mutex);
      _events.raise(client_disconnected, this, channel);
      auto removed = _clients.extract(channel);
      _client_parking_space = move(removed.mapped());
      });
    channel->open();

    unique_lock<shared_mutex> lock(_mutex);
    _clients.emplace(channel.get(), move(channel));
  }
}