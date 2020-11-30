#include "pch.h"
#include "MessagingClient.h"

using namespace Lux::Events;
using namespace Lux::Threading;
using namespace std;

namespace Lux::Networking
{
  messaging_client::messaging_client() :
    connected(_events),
    is_connected_changed(_events)
  { }

  messaging_client::~messaging_client()
  {
    _is_disposed.set();
  }

  void messaging_client::on_opening()
  {
    _connection_thread = make_unique<background_thread>([&] { connect(); }, L"Messaging connection thread");
  }

  message_task messaging_client::send(Serialization::memory_stream&& message)
  {
    lock_guard<mutex> lock(_mutex);
    if (_channel)
    {
      return _channel->send(move(message));
    }
    else
    {
      return message_task{};
    }
  }

  bool messaging_client::is_connected() const
  {
    return _channel != nullptr;
  }

  void messaging_client::connect()
  {
    while (!_is_disposed.wait(0ms))
    {
      try
      {
        auto channel = get_client();

        manual_reset_event event;
        {
          {
            lock_guard<mutex> lock(_mutex);
            _channel = move(channel);
          }
          _events.raise(connected, this, _channel.get());
          _events.raise(is_connected_changed, this, true);

          _channel->disconnected(no_revoke, [&](messaging_channel*) { event.set(); });
          _channel->open();
        }

        reset_event_base::wait_any({ &event, &_is_disposed });
      }
      catch (...)
      {
        //log
      }

      {
        lock_guard<mutex> lock(_mutex);
        _channel.reset();
      }
      _events.raise(is_connected_changed, this, false);
    }
  }
}