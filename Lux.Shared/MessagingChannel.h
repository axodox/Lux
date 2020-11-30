#pragma once
#include "MemoryStream.h"
#include "Events.h"
#include "BlockingCollection.h"
#include "Openable.h"

namespace Lux::Networking
{
  class message_promise
  {
  private:
    Serialization::memory_stream _message;
    std::promise<bool> _promise;
    std::mutex _mutex;
    bool _is_canceled = false;
    bool _is_finished = false;
    bool _is_succeeded = false;

  public:
    message_promise() = default;

    explicit message_promise(Serialization::memory_stream&& message);

    const Serialization::memory_stream& message() const;

    void cancel();
    bool is_canceled() const;

    void finish(bool success);
    bool is_finished() const;
    bool is_succeeded() const;

    std::future<bool> get_future();

    ~message_promise();
  };

  class message_task
  {
  private:
    std::weak_ptr<message_promise> _message_promise;

  public:
    std::future<bool> future;

    void cancel();

    message_task(const std::shared_ptr<message_promise>& messagePromise);

    message_task();
  };

  class messaging_channel : public Infrastructure::openable
  {  
    friend class messaging_server;
    friend class messaging_client;
    Events::event_owner _events;

  public:
    messaging_channel();
    
    bool is_connected() const;
    Events::event_publisher<messaging_channel*> disconnected;

    virtual message_task send(Serialization::memory_stream&& message) = 0;
    Events::event_publisher<messaging_channel*, Serialization::memory_stream&&> received;

  protected:
    void on_received(Serialization::memory_stream&& message);
    void on_disconnected();

  private:
    std::mutex _connection_mutex;
    bool _is_connected = true;
  };
}