#pragma once
#include "MessagingClient.h"

namespace Lux::Networking
{
  struct ip_endpoint
  {
    std::wstring hostname;
    uint16_t port;
  };

  class tcp_messaging_client final : public messaging_client
  {
  public:
    tcp_messaging_client(ip_endpoint const& endpoint);

    ip_endpoint endpoint() const;

  protected:
    virtual std::unique_ptr<messaging_channel> get_client() override;
  
  private:
    ip_endpoint _endpoint;
  };
}
