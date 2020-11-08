#pragma once
#include "LightController.h"
#include "DeviceSettings.h"

namespace Lux::Controllers
{
  class AdaLightController : public LightController
  {
  public:
    AdaLightController() = default;
    virtual ~AdaLightController();

    const Configuration::AdaLightSettings& Options() const;
    void Options(const Configuration::AdaLightSettings& options);

    virtual void Push(const std::vector<Graphics::rgb>& colors) override;

  private:
    Configuration::AdaLightSettings _options{};

    std::mutex _mutex;
    winrt::Windows::Devices::Enumeration::DeviceWatcher _deviceWatcher{ nullptr };
    winrt::hstring _currentDeviceId;

    winrt::Windows::Storage::Streams::DataWriter _serialWriter{ nullptr };
    std::chrono::steady_clock::time_point _lastUpdate = {};

    void OnDeviceAdded(const winrt::Windows::Devices::Enumeration::DeviceWatcher&, const winrt::Windows::Devices::Enumeration::DeviceInformation& deviceInformation);
    void OnDeviceRemoved(const winrt::Windows::Devices::Enumeration::DeviceWatcher&, const winrt::Windows::Devices::Enumeration::DeviceInformationUpdate& deviceInformation);
  };
}