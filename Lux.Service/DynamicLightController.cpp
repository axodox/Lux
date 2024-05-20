#include "pch.h"
#include "DynamicLightController.h"
#include "MemoryStream.h"

using namespace Lux::Configuration;
using namespace Lux::Graphics;
using namespace Lux::Serialization;

using namespace std;
using namespace std::this_thread;
using namespace std::chrono;

using namespace winrt;
using namespace winrt::Windows::Foundation::Numerics;
using namespace winrt::Windows::Devices::Enumeration;
using namespace winrt::Windows::Devices::Lights;
using namespace winrt::Windows::Devices::Usb;
using namespace winrt::Windows::UI;
using namespace winrt::Windows::Storage::Streams;

namespace Lux::Controllers
{
  DynamicLightController::~DynamicLightController()
  {
    _deviceWatcher.Stop();
  }

  const Configuration::DynamicLightSettings& DynamicLightController::Options() const
  {
    return _options;
  }

  void DynamicLightController::Options(const Configuration::DynamicLightSettings& options)
  {
    lock_guard<mutex> lock(_mutex);
    _options = options;

    _currentDeviceId = {};
    _lampArray = { nullptr };

    auto deviceSelector = LampArray::GetDeviceSelector();
    _deviceWatcher = DeviceInformation::CreateWatcher(deviceSelector);
    _deviceWatcher.Added({ this, &DynamicLightController::OnDeviceAdded });
    _deviceWatcher.Updated([](const auto&...) {}); //Device watcher requires all events to be bound to work
    _deviceWatcher.Removed({ this, &DynamicLightController::OnDeviceRemoved });
    _deviceWatcher.Start();
  }

  void DynamicLightController::Push(const vector<rgb>& colors)
  {
    //Check if we have a connection
    lock_guard<mutex> lock(_mutex);
    if (!_lampArray) return;;

    //Collect colors
    std::vector<Color> lampColors;
    lampColors.reserve(colors.size());

    std::vector<int32_t> lampIds;
    lampIds.reserve(colors.size());

    for (auto i = 0; auto& color : colors)
    {
      lampColors.push_back(Color{ .A = 255, .R = color.r, .G = color.g, .B = color.b });
      lampIds.push_back(i++);
    }
    
    //Rate limit for LED refresh
    auto now = steady_clock::now();
    auto timeSinceLastUpdate = now - _lastUpdate;
    if (timeSinceLastUpdate < _lampArray.MinUpdateInterval())
    {
      sleep_for(_lampArray.MinUpdateInterval() - timeSinceLastUpdate);
      _lastUpdate = steady_clock::now();
    }
    else
    {
      _lastUpdate = now;
    }

    //Push message
    try
    {
      _lampArray.SetColorsForIndices(lampColors, lampIds);
    }
    catch (...)
    {
      //log
    }
  }

  Configuration::LightControllerKind DynamicLightController::Type() const
  {
    return LightControllerKind::Dynamic;
  }

  void DynamicLightController::OnDeviceAdded(const DeviceWatcher&, const DeviceInformation& deviceInformation)
  {
    lock_guard<mutex> lock(_mutex);
    if (_lampArray) return;

    try
    {
      auto currentDeviceId = deviceInformation.Id();
      auto lampArray = LampArray::FromIdAsync(deviceInformation.Id()).get();

      if (lampArray.HardwareVendorId() != _options.UsbVendorId || lampArray.HardwareProductId() != _options.UsbProductId) return;

      _currentDeviceId = currentDeviceId;
      _lampArray = lampArray;
      IsConnected(true);
    }
    catch (...)
    {
      _currentDeviceId = {};
      _lampArray = { nullptr };
    }
  }

  void DynamicLightController::OnDeviceRemoved(const DeviceWatcher&, const DeviceInformationUpdate& deviceInformation)
  {
    lock_guard<mutex> lock(_mutex);
    if (!_lampArray || _currentDeviceId != deviceInformation.Id()) return;

    _currentDeviceId = {};
    _lampArray = { nullptr };
    IsConnected(false);
  }
}