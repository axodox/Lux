#pragma once
#include "MessagingChannel.h"
#include "BackgroundThread.h"
#include "Openable.h"

namespace Lux::Networking
{
  class messaging_client : public Infrastructure::openable
  {
  private:
    Events::event_owner _events;

  public:
    messaging_client();

    Events::event_publisher<messaging_client*, messaging_channel*> connected;

    message_task send(Serialization::memory_stream&& message);

    bool is_connected() const;
    Events::event_publisher<messaging_client*, bool> is_connected_changed;

    virtual ~messaging_client();

  protected:
    virtual void on_opening() override;
    virtual std::unique_ptr<messaging_channel> get_client() = 0;

  private:
    std::mutex _mutex;
    std::unique_ptr<messaging_channel> _channel;
    std::unique_ptr<Threading::background_thread> _connection_thread;
    Threading::manual_reset_event _is_disposed;

    void connect();
  };
}