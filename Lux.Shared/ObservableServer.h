#pragma once
#include "Observable.h"
#include "MessagingServer.h"

namespace Lux::Observable
{
  template<typename T, typename = std::enable_if_t<std::is_convertible<T*, observable*>::value>>
  class observable_server
  {
  private:
    std::unique_ptr<Networking::messaging_server> _messagingServer;
    observable_root<T> _root;

    void on_client_connected(Networking::messaging_channel* channel)
    {
      Serialization::memory_stream stream;
      stream.write(_root);

      
    }

  public:
    observable_server(std::unique_ptr<Networking::messaging_server>&& messagingServer) :
      _messagingServer(messagingServer)
    {
      _messagingServer->client_connected(Events::no_revoke, [&](Networking::messaging_server*, Networking::messaging_channel* channel) { on_client_connected(channel); });
    }
  };
}