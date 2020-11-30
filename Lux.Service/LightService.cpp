#include "pch.h"
#include "LightService.h"
#include "IO.h"
#include "FileStream.h"
#include "StaticSource.h"
#include "RainbowSource.h"
#include "DesktopSource.h"
#include "DisplaySettings.h"

using namespace Lux;
using namespace Lux::Configuration;
using namespace Lux::Events;
using namespace Lux::Observable;
using namespace Lux::Serialization;
using namespace Lux::Controllers;
using namespace Lux::Sources;
using namespace Lux::Colors;

using namespace std;
using namespace std::filesystem;

using namespace winrt;
using namespace winrt::Windows::System::Threading;

namespace Lux::Service
{
  LightService::LightService() :
    _settingsPath{ IO::app_folder() / L"settings.bin" },
    _server{ make_unique<Networking::tcp_messaging_server>(Configuration::LuxPort), make_unique<Threading::simple_dispatcher>() },
    _timer{ ThreadPoolTimer::CreatePeriodicTimer({ this, &LightService::SaveSettings }, 1000ms) },
    _controller(make_unique<AdaLightController>())
  {
    _colorProcessors.push_back(&_colorCorrector);

    _controller->IsConnectedChanged(no_revoke, [&](LightController*) { 
      _server.root()->IsConnected = _controller->IsConnected();
      });

    LoadSettings();

    _server.root()->property_changed(no_revoke, member_func(this, &LightService::OnSettingChanged));
    _server.open();
  }

  void LightService::LoadSettings()
  {
    error_code error;
    bool isLoaded = false;
    if (exists(_settingsPath, error))
    {
      try
      {
        file_stream stream{ _settingsPath, file_mode::read };
        _server.root()->deserialize(stream);
        isLoaded = true;
      }
      catch (...)
      {
        //log
      }
    }

    if (!isLoaded)
    {
      _isDirty = true; //Trigger writing default settings
    }

    ApplyContollerSettings();
    ApplySourceSettings();

    ApplyColorCorrectorSettings();
  }

  void LightService::ApplyColorCorrectorSettings()
  {
    _colorCorrector.Saturation(_server.root()->Saturation);
    _colorCorrector.Brightness(_server.root()->Brightness / 255.f);
    _colorCorrector.MaxBrightness(_server.root()->BrightnessLimit / 255.f);
    _colorCorrector.Gamma(_server.root()->Gamma);
  }

  void LightService::SaveSettings(const ThreadPoolTimer& /*timer*/)
  {
    if (!_isDirty) return;
    _isDirty = false;

    _server.dispatcher()->invoke([&] {
      try
      {
        file_stream stream{ _settingsPath, file_mode::write };
        _server.root()->serialize(stream);
      }
      catch (...)
      {
        //log
      }
      });
  }


  void LightService::ApplyContollerSettings()
  {
    _controller->Options(_server.root()->Device.value().AdaLight);
  }

  void LightService::ApplySourceSettings()
  {
    auto sourceKind = _server.root()->LightSource.value();

    auto isNew = false;
    if (sourceKind == LightSourceKind::Off)
    {
      _source.reset();
    }
    else if(!_source || _source->Kind() != sourceKind)
    {
      _source.reset();

      switch (sourceKind)
      {
      case LightSourceKind::Static:
        _source = make_unique<StaticSource>();
        isNew = true;
        break;
      case LightSourceKind::Rainbow:
        _source = make_unique<RainbowSource>();
        isNew = true;
        break;
      case LightSourceKind::Desktop:
        _source = make_unique<DesktopSource>();
        isNew = true;
        break;
      }
    }

    if (isNew)
    {
      auto displaySettings = DisplaySettings::FromLayout(_server.root()->Device.value().Layout);

      _source->Settings(move(displaySettings));
      _source->ColorsEmitted(no_revoke, member_func(this, &LightService::OnColorsEmitted));

      switch (sourceKind)
      {
      case LightSourceKind::Static:
        ApplyStaticSourceSettings();
        break;
      case LightSourceKind::Rainbow:
        ApplyRainbowSourceSettings();
        break;
      case LightSourceKind::Desktop:
        ApplyDesktopSourceSettings();
        break;
      }
    }
  }

  void LightService::OnColorsEmitted(Sources::LightSource* /*source*/, std::vector<Graphics::rgb>&& colors)
  {
    if (_mutex.try_lock())
    {
      try
      {
        auto output = move(colors);

        //Apply color processors
        for (auto& processor : _colorProcessors)
        {
          processor->ProcessColors(output);
        }

        _controller->Push(output);
      }
      catch (...)
      {
        //log
      }
      _mutex.unlock();
    }
  }

  void LightService::OnSettingChanged(observable_object<LightConfigurationProperty>* /*object*/, LightConfigurationProperty propertyKey)
  {
    _isDirty = true;

    switch (propertyKey)
    {
    case LightConfigurationProperty::Device:
    {
      lock_guard<mutex> lock(_mutex);
      ApplyContollerSettings();
      ApplySourceSettings();
      break;
    }
    case LightConfigurationProperty::LightSource:
    {
      lock_guard<mutex> lock(_mutex);
      ApplySourceSettings();
      break;
    }
    case LightConfigurationProperty::StaticSourceOptions:
      ApplyStaticSourceSettings();
      break;
    case LightConfigurationProperty::RainbowSourceOptions:
      ApplyRainbowSourceSettings();
      break;
    case LightConfigurationProperty::DesktopSourceOptions:
      ApplyDesktopSourceSettings();
      break;
    case LightConfigurationProperty::Saturation:
      _colorCorrector.Saturation(_server.root()->Saturation);
      break; 
    case LightConfigurationProperty::Brightness:
      _colorCorrector.Brightness(_server.root()->Brightness / 255.f);
      break;
    case LightConfigurationProperty::BrightnessLimit:
      _colorCorrector.MaxBrightness(_server.root()->BrightnessLimit / 255.f);
      break;
    case LightConfigurationProperty::Gamma:
      _colorCorrector.Gamma(_server.root()->Gamma);
      break;
    default:

      break;
    }
  }

  void LightService::ApplyStaticSourceSettings()
  {
    if (_source && _source->Kind() == LightSourceKind::Static)
    {
      auto staticSource = static_cast<StaticSource*>(_source.get());
      staticSource->Color(_server.root()->StaticSourceOptions->Color);
    }
  }

  void LightService::ApplyRainbowSourceSettings()
  {
    if (_source && _source->Kind() == LightSourceKind::Rainbow)
    {
      auto rainbowSource = static_cast<RainbowSource*>(_source.get());
      rainbowSource->SpatialFrequency(_server.root()->RainbowSourceOptions->SpatialFrequency);
      rainbowSource->AngularVelocity(_server.root()->RainbowSourceOptions->AngularVelocity);
    }
  }

  void LightService::ApplyDesktopSourceSettings()
  {
    if (_source && _source->Kind() == LightSourceKind::Desktop)
    {
      auto desktopSource = static_cast<DesktopSource*>(_source.get());
      desktopSource->TemporalAveraging(_server.root()->DesktopSourceOptions->TemporalAveraging);
      desktopSource->SampleSize(_server.root()->DesktopSourceOptions->SampleSize);
    }
  }

  void LightService::Run()
  {
    MSG message;
    while (GetMessage(&message, NULL, 0, 0) > 0)
    {
      TranslateMessage(&message);
      DispatchMessage(&message);
    }
  }
}