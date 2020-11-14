#pragma once
#include "LightConfiguration.h"
#include "TcpMessagingServer.h"
#include "Dispatcher.h"
#include "ObservableServer.h"
#include "NetworkSettings.h"
#include "AdaLightController.h"
#include "LightSource.h"
#include "ColorProcessor.h"

namespace Lux::Service
{
  class LightService
  {
  private:
    std::filesystem::path _settingsPath;
    Observable::observable_server<Configuration::LightConfiguration> _server;
    winrt::Windows::System::Threading::ThreadPoolTimer _timer;
    bool _isDirty = false;
    
    void LoadSettings();
    void SaveSettings(const winrt::Windows::System::Threading::ThreadPoolTimer& timer);

    void OnSettingChanged(Observable::observable_object<Configuration::LightConfigurationProperty>* object, Configuration::LightConfigurationProperty propertyKey);

    void ApplyStaticSourceSettings();
    void ApplyRainbowSourceSettings();

    std::unique_ptr<Controllers::AdaLightController> _controller;
    std::unique_ptr<Sources::LightSource> _source;
    std::vector<std::unique_ptr<Colors::ColorProcessor>> _colorProcessors;
    std::mutex _mutex;

    void ApplyContollerSettings();
    void ApplySourceSettings();

    void OnColorsEmitted(Sources::LightSource* source, std::vector<Graphics::rgb>&& colors);

  public:
    LightService();
    void Run();
  };
}