#pragma once
#include "Observable.h"
#include "MessagingServer.h"
#include "Dispatcher.h"

namespace Lux::Observable
{
  template<typename T, typename = std::enable_if_t<std::is_convertible<T*, observable*>::value>>
  class observable_server : public Infrastructure::openable
  {
  private:
    std::unique_ptr<Networking::messaging_server> _messaging_server;
    std::unique_ptr<Threading::dispatcher> _dispatcher;
    observable_root<T> _root;
    Networking::messaging_channel* _active_channel;

    void on_message_received(Networking::messaging_channel* channel, Serialization::memory_stream&& message)
    {
      auto change = message.read<std::unique_ptr<Observable::change>>();

      _dispatcher->invoke([&] {
        _active_channel = channel;
        _root.apply_change(change.get());
        _active_channel = nullptr;
        }).get();
    }

    void on_client_connected(Networking::messaging_channel* channel)
    {
      Serialization::memory_stream stream;
      _dispatcher->invoke([&] { stream.write(_root); }).get();

      channel->received(Events::no_revoke, [&](Networking::messaging_channel* channel, Serialization::memory_stream&& message) { on_message_received(channel, std::move(message)); });
      channel->open();
      channel->send(std::move(stream));
    }

    void on_change_reported(std::unique_ptr<change>&& change)
    {
      Serialization::memory_stream stream;
      stream.write(change);

      _messaging_server->broadcast(std::move(stream), _active_channel);
    }

  protected:
    virtual void on_opening() override
    {
      _messaging_server->open();
    }

  public:
    observable_server(
      std::unique_ptr<Networking::messaging_server>&& messagingServer,
      std::unique_ptr<Threading::dispatcher>&& dispatcher) :
      _messaging_server(std::move(messagingServer)),
      _dispatcher(std::move(dispatcher))
    {
      _root.change_reported(Events::no_revoke, [&](observable_root<T>*, std::unique_ptr<change>&& change) { on_change_reported(std::move(change)); });
      _messaging_server->client_connected(Events::no_revoke, [&](Networking::messaging_server*, Networking::messaging_channel* channel) { on_client_connected(channel); });
    }

    Threading::dispatcher* dispatcher()
    {
      return _dispatcher.get();
    }

    T* root()
    {
      return &_root;
    }
  };
}