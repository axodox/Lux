#include "pch.h"
#include "AdaLightController.h"
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
using namespace winrt::Windows::Devices::SerialCommunication;
using namespace winrt::Windows::Storage::Streams;

namespace Lux::Controllers
{
  AdaLightController::~AdaLightController()
  {
    _deviceWatcher.Stop();
  }

  const Configuration::AdaLightSettings& AdaLightController::Options() const
  {
    return _options;
  }

  void AdaLightController::Options(const Configuration::AdaLightSettings& options)
  {
    lock_guard<mutex> lock(_mutex);
    _options = options;

    _currentDeviceId = {};
    _serialWriter = { nullptr };

    auto deviceSelector = SerialDevice::GetDeviceSelectorFromUsbVidPid(options.UsbVendorId, options.UsbProductId);
    _deviceWatcher = DeviceInformation::CreateWatcher(deviceSelector);
    _deviceWatcher.Added({ this, &AdaLightController::OnDeviceAdded });
    _deviceWatcher.Updated([](const auto&...) {}); //Device watcher requires all events to be bound to work
    _deviceWatcher.Removed({ this, &AdaLightController::OnDeviceRemoved });
    _deviceWatcher.Start();
  }

  void AdaLightController::Push(const vector<rgb>& colors)
  {
    //Check if we have a connection
    lock_guard<mutex> lock(_mutex);
    if (!_serialWriter) return;;

    //Define message
    auto length = 6 + colors.size() * 3;

    memory_stream messsage;
    messsage.reserve(length);

    messsage.write(uint8_t(0x41));
    messsage.write(uint8_t(0x64));
    messsage.write(uint8_t(0x61));

    auto adjustedLedCount = colors.size() - 1;
    auto highCount = (uint8_t)(adjustedLedCount >> 8);
    auto lowCount = (uint8_t)(adjustedLedCount & 0xff);
    auto checksumCount = (uint8_t)(highCount ^ lowCount ^ 0x55);

    messsage.write(highCount);
    messsage.write(lowCount);
    messsage.write(checksumCount);

    uint32_t checksum = 0;
    for (auto& color : colors)
    {
      messsage.write(color.g);
      messsage.write(color.r);
      messsage.write(color.b);

      uint32_t hash = (uint32_t(color.r) << 24) + (uint32_t(color.g) << 16) + uint32_t(color.b);
      checksum = (checksum + 0x9e3779b9 + (hash << 6) + (hash >> 2)) ^ hash;
    }

    messsage.write(checksum);

    //Rate limit for LED refresh
    auto now = steady_clock::now();
    auto timeSyncLastUpdate = now - _lastUpdate;
    if (timeSyncLastUpdate < _options.LedSyncDuration)
    {
      sleep_for(_options.LedSyncDuration - timeSyncLastUpdate);
      _lastUpdate = steady_clock::now();
    }
    else
    {
      _lastUpdate = now;
    }    

    //Push message
    try
    {
      _serialWriter.WriteBytes(messsage);
      _serialWriter.StoreAsync().get();
    }
    catch (...)
    {
      //log
    }
  }

  Configuration::LightControllerKind AdaLightController::Type() const
  {
    return LightControllerKind::Ada;
  }

  void AdaLightController::OnDeviceAdded(const DeviceWatcher&, const DeviceInformation& deviceInformation)
  {
    lock_guard<mutex> lock(_mutex);
    if (_serialWriter) return;

    try
    {
      _currentDeviceId = deviceInformation.Id();
      auto serialDevice = SerialDevice::FromIdAsync(deviceInformation.Id()).get();
      serialDevice.BaudRate(_options.BaudRate);
      serialDevice.IsRequestToSendEnabled(false);
      serialDevice.IsDataTerminalReadyEnabled(false);
      _serialWriter = DataWriter(serialDevice.OutputStream());
      IsConnected(true);
    }
    catch (...)
    {
      _currentDeviceId = {};
      _serialWriter = { nullptr };
    }
  }

  void AdaLightController::OnDeviceRemoved(const DeviceWatcher&, const DeviceInformationUpdate& deviceInformation)
  {
    lock_guard<mutex> lock(_mutex);
    if (!_serialWriter || _currentDeviceId != deviceInformation.Id()) return;

    _currentDeviceId = {};
    _serialWriter = { nullptr };
    IsConnected(false);
  }
}