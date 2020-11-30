#pragma once
#include "LightConfiguration.h"
#include "TcpMessagingServer.h"
#include "Dispatcher.h"
#include "ObservableServer.h"
#include "NetworkSettings.h"
#include "AdaLightController.h"
#include "LightSource.h"
#include "ColorProcessor.h"
#include "ColorCorrector.h"

namespace Lux::Service
{
  class LightService
  {
  private:
    std::filesystem::path _settingsPath;
    Observable::observable_server<Configuration::LightConfiguration> _server;
    winrt::Windows::System::Threading::ThreadPoolTimer _timer;
    bool _isDirty = false;
    
    std::unique_ptr<Controllers::AdaLightController> _controller;
    std::unique_ptr<Sources::LightSource> _source;
    Colors::ColorCorrector _colorCorrector;
    std::vector<Colors::ColorProcessor*> _colorProcessors;
    std::mutex _mutex;

    void LoadSettings();
    void ApplyColorCorrectorSettings();
    void SaveSettings(const winrt::Windows::System::Threading::ThreadPoolTimer& timer);

    void OnSettingChanged(Observable::observable_object<Configuration::LightConfigurationProperty>* object, Configuration::LightConfigurationProperty propertyKey);

    void ApplyStaticSourceSettings();
    void ApplyRainbowSourceSettings();
    void ApplyDesktopSourceSettings();

    void ApplyContollerSettings();
    void ApplySourceSettings();

    void OnColorsEmitted(Sources::LightSource* source, std::vector<Graphics::rgb>&& colors);

  public:
    LightService();
    void Run();
  };
}