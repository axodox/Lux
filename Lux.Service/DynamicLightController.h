#pragma once
#include "LightController.h"
#include "DeviceSettings.h"

namespace Lux::Controllers
{
  class DynamicLightController : public LightController
  {
  public:
    DynamicLightController() = default;
    virtual ~DynamicLightController();

    const Configuration::DynamicLightSettings& Options() const;
    void Options(const Configuration::DynamicLightSettings& options);

    virtual void Push(const std::vector<Graphics::rgb>& colors) override;
    virtual Configuration::LightControllerKind Type() const override;

  private:
    Configuration::DynamicLightSettings _options{};

    std::mutex _mutex;
    winrt::Windows::Devices::Enumeration::DeviceWatcher _deviceWatcher{ nullptr };
    winrt::hstring _currentDeviceId;

    winrt::Windows::Devices::Lights::LampArray _lampArray{ nullptr };
    std::chrono::steady_clock::time_point _lastUpdate = {};

    void OnDeviceAdded(const winrt::Windows::Devices::Enumeration::DeviceWatcher&, const winrt::Windows::Devices::Enumeration::DeviceInformation& deviceInformation);
    void OnDeviceRemoved(const winrt::Windows::Devices::Enumeration::DeviceWatcher&, const winrt::Windows::Devices::Enumeration::DeviceInformationUpdate& deviceInformation);
  };
}