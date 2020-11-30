#pragma once
#include "Observable.h"
#include "MessagingClient.h"
#include "Dispatcher.h"

namespace Lux::Observable
{
  template<typename T, typename = std::enable_if_t<std::is_convertible<T*, observable*>::value>>
  class observable_client : public Infrastructure::openable
  {
  private:
    Events::event_owner _events;

  public:
    Events::event_publisher<observable_client<T>*> is_connected_changed, full_data_reset;

  private:
    std::shared_ptr<Threading::dispatcher> _dispatcher;
    std::unique_ptr<Networking::messaging_client> _messaging_client;
    observable_root<T> _root;

    bool _is_initializing = false;
    bool _is_refreshing = false;

    void on_message_received(Serialization::memory_stream&& message)
    {
      _is_refreshing = true;
      if (_is_initializing)
      {
        _dispatcher->invoke([&] { message.read(_root); }).get();

        _events.raise(full_data_reset, this);
        _is_initializing = false;
      }
      else
      {
        auto change = message.read<std::unique_ptr<Observable::change>>();

        _dispatcher->invoke([&] {
          _root.apply_change(change.get());
          }).get();
      }
      _is_refreshing = false;
    }

    void on_connected(Networking::messaging_channel* channel)
    {
      _is_initializing = true;
      channel->received(Events::no_revoke, [&](Networking::messaging_channel* /*channel*/, Serialization::memory_stream&& message) { on_message_received(std::move(message)); });
      channel->open();
    }

    void on_change_reported(std::unique_ptr<change>&& change)
    {
      if (_is_refreshing) return;

      Serialization::memory_stream stream;
      stream.write(change);

      _messaging_client->send(std::move(stream));
    }

  protected:
    virtual void on_opening() override
    {
      _messaging_client->open();
    }

  public:
    observable_client(
      std::unique_ptr<Networking::messaging_client>&& messagingServer,
      const std::shared_ptr<Threading::dispatcher>& dispatcher) :
      _messaging_client(std::move(messagingServer)),
      _dispatcher(dispatcher),
      is_connected_changed(_events),
      full_data_reset(_events)
    {
      _root.change_reported(Events::no_revoke, [&](observable_root<T>*, std::unique_ptr<change>&& change) { on_change_reported(std::move(change)); });
      _messaging_client->connected(Events::no_revoke, [&](Networking::messaging_client*, Networking::messaging_channel* channel) { on_connected(channel); });
      _messaging_client->is_connected_changed(Events::no_revoke, [&](auto...) {
        _events.raise(is_connected_changed, this);
        });
    }

    Threading::dispatcher* dispatcher()
    {
      return &_dispatcher;
    }

    T* root()
    {
      if (!_dispatcher->has_access()) throw winrt::hresult_wrong_thread();
      return &_root;
    }

    bool is_connected() const
    {
      return _messaging_client->is_connected();
    }
  };
}