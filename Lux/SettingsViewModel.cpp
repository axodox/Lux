#include "pch.h"
#include "SettingsViewModel.h"
#include "SettingsViewModel.g.cpp"

#include "DependencyConfiguration.h"
#include "ObservableClient.h"

using namespace ::Lux;
using namespace ::Lux::Configuration;
using namespace ::Lux::Events;
using namespace ::Lux::Observable;
using namespace ::Lux::Threading;

using namespace winrt;
using namespace winrt::Windows::Foundation::Numerics;
using namespace winrt::Windows::UI::Xaml::Data;

namespace winrt::Lux::implementation
{
  SettingsViewModel::SettingsViewModel() :
    _client(DependencyConfiguration::Instance().resolve<observable_client<LightConfiguration>>()),
    _dispatcher(DependencyConfiguration::Instance().resolve<dispatcher>())
  {
    _client->full_data_reset(no_revoke, member_func(this, &SettingsViewModel::OnFullDataReset));
    _client->root()->property_changed(no_revoke, member_func(this, &SettingsViewModel::OnSettingChanged));
  }

  float SettingsViewModel::GammaR()
  {
    return _client->root()->Gamma->x;
  }

  void SettingsViewModel::GammaR(float value)
  {
    if (_client->root()->Gamma->x == value) return;
    
    if (_isGammaLocked)
    {
      _client->root()->Gamma = float3(value);
    }
    else
    {
      auto gamma = *_client->root()->Gamma;
      gamma.x = value;
      _client->root()->Gamma = gamma;
    }
  }

  float SettingsViewModel::GammaG()
  {
    return _client->root()->Gamma->y;
  }

  void SettingsViewModel::GammaG(float value)
  {
    if (_client->root()->Gamma->y == value) return;
    
    if (_isGammaLocked)
    {
      _client->root()->Gamma = float3(value);
    }
    else
    {
      auto gamma = *_client->root()->Gamma;
      gamma.y = value;
      _client->root()->Gamma = gamma;
    }
  }

  float SettingsViewModel::GammaB()
  {
    return _client->root()->Gamma->z;
  }

  void SettingsViewModel::GammaB(float value)
  {
    if (_client->root()->Gamma->z == value) return;

    if (_isGammaLocked)
    {
      _client->root()->Gamma = float3(value);
    }
    else
    {
      auto gamma = *_client->root()->Gamma;
      gamma.z = value;
      _client->root()->Gamma = gamma;
    }
  }

  bool SettingsViewModel::IsGammaLocked()
  {
    return _isGammaLocked;
  }

  void SettingsViewModel::IsGammaLocked(bool value)
  {
    if (_isGammaLocked == value) return;

    _isGammaLocked = value;
    _propertyChanged(*this, PropertyChangedEventArgs(L"IsGammaLocked"));
  }

  uint8_t SettingsViewModel::BrightnessLimit()
  {
    return _client->root()->BrightnessLimit;
  }

  void SettingsViewModel::BrightnessLimit(uint8_t value)
  {
    if (_client->root()->BrightnessLimit == value) return;
    _client->root()->BrightnessLimit = value;
  }

  float SettingsViewModel::Saturation()
  {
    return _client->root()->Saturation;
  }

  void SettingsViewModel::Saturation(float value)
  {
    if (_client->root()->Saturation == value) return;
    _client->root()->Saturation = value;
  }

  winrt::event_token SettingsViewModel::PropertyChanged(PropertyChangedEventHandler const& handler)
  {
    return _propertyChanged.add(handler);
  }

  void SettingsViewModel::PropertyChanged(event_token const& token) noexcept
  {
    _propertyChanged.remove(token);
  }

  void SettingsViewModel::OnFullDataReset(observable_client<LightConfiguration>* /*sender*/)
  {
    _dispatcher->invoke([&] {
      _propertyChanged(*this, PropertyChangedEventArgs(L""));
      });
  }

  void SettingsViewModel::OnSettingChanged(observable_object<LightConfigurationProperty>* object, LightConfigurationProperty propertyKey)
  {
    switch (propertyKey)
    {
    case LightConfigurationProperty::Gamma:
      _propertyChanged(*this, PropertyChangedEventArgs(L"GammaR"));
      _propertyChanged(*this, PropertyChangedEventArgs(L"GammaG"));
      _propertyChanged(*this, PropertyChangedEventArgs(L"GammaB"));
      break;
    case LightConfigurationProperty::BrightnessLimit:
      _propertyChanged(*this, PropertyChangedEventArgs(L"BrightnessLimit"));
      break;
    case LightConfigurationProperty::Saturation:
      _propertyChanged(*this, PropertyChangedEventArgs(L"Saturation"));
      break;
    }
  }
}
