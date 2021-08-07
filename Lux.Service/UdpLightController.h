#pragma once
#include "LightController.h"
#include "DeviceSettings.h"

namespace Lux::Controllers
{
  class UdpLightController : public LightController
  {
  public:
    UdpLightController() = default;

    const Configuration::UdpLightSettings& Options() const;
    void Options(const Configuration::UdpLightSettings& options);

    virtual void Push(const std::vector<Graphics::rgb>& colors) override;
    virtual Configuration::LightControllerKind Type() const override;

  private:
    Configuration::UdpLightSettings _options{};
    winrt::Windows::Networking::Sockets::DatagramSocket _socket;
    std::unique_ptr<Serialization::stream> _stream;
    std::vector<uint8_t> _buffer;
    uint32_t _messageId = 0;

    void Connect();
  };
}