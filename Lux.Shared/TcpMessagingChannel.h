#pragma once
#include "MessagingChannel.h"
#include "BlockingCollection.h"
#include "BackgroundThread.h"

namespace Lux::Networking
{
  class tcp_messaging_channel final : public messaging_channel
  {
    friend class tcp_messaging_server;
    friend class tcp_messaging_client;
    Events::event_owner _events;

  public:
    Events::event_publisher<tcp_messaging_channel*> disconnected;
    virtual message_task send(Serialization::memory_stream&& message) override;

    virtual ~tcp_messaging_channel();

  protected:
    virtual void on_opening() override;

  private:
    static const uint64_t _magic = 0x0123456789ABCDEF;
    winrt::Windows::Networking::Sockets::StreamSocket _socket;
    Threading::blocking_collection<std::shared_ptr<message_promise>> _messages_to_send;
    bool _is_open = false;

    std::unique_ptr<Threading::background_thread> _send_thread, _receive_thread;

    tcp_messaging_channel(const winrt::Windows::Networking::Sockets::StreamSocket& socket);

    void send();
    void receive();
  };
}