#include "pch.h"
#include "MessagingChannel.h"

using namespace std;

namespace Lux::Networking
{
  message_promise::message_promise(Serialization::memory_stream&& message) :
    _message(move(message))
  { }

  const Serialization::memory_stream& message_promise::message() const
  {
    return _message;
  }

  void message_promise::cancel()
  {
    lock_guard<mutex> lock(_mutex);
    if (_is_finished) return;

    _is_finished = true;
    _is_canceled = true;
    _is_succeeded = false;
    _promise.set_value(false);
  }

  bool message_promise::is_canceled() const
  {
    return _is_canceled;
  }

  void message_promise::finish(bool success)
  {
    lock_guard<mutex> lock(_mutex);
    if (_is_finished) return;

    _is_finished = true;
    _is_succeeded = success;
    _promise.set_value(success);
  }

  bool message_promise::is_finished() const
  {
    return _is_finished;
  }

  bool message_promise::is_succeeded() const
  {
    return _is_succeeded;
  }

  std::future<bool> message_promise::get_future()
  {
    return _promise.get_future();
  }

  message_promise::~message_promise()
  {
    lock_guard<mutex> lock(_mutex);
    if (!_is_finished)
    {
      _promise.set_value(false);
    }
  }

  void message_task::cancel()
  {
    auto messagePromise = _message_promise.lock();
    if (messagePromise)
    {
      messagePromise->cancel();
    }
  }

  message_task::message_task(const std::shared_ptr<message_promise>& messagePromise) :
    _message_promise(messagePromise),
    future(messagePromise->get_future())
  { }

  message_task::message_task()
  {
    std::promise<bool> promise;
    future = promise.get_future();
    promise.set_value(false);
  }

  messaging_channel::messaging_channel() :
    disconnected(_events),
    received(_events)    
  { }

  bool messaging_channel::is_connected() const
  {
    return _is_connected;
  }

  void messaging_channel::on_received(Serialization::memory_stream && message)
  {
    _events.raise(received, this, std::move(message));
  }

  void messaging_channel::on_disconnected()
  {
    if (!_is_connected) return;

    lock_guard<mutex> lock(_connection_mutex);
    if (_is_connected)
    {
      _is_connected = false;
      _events.raise(disconnected, this);
    }
  }
}