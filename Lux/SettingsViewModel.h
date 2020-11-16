#pragma once
#include "SettingsViewModel.g.h"

#include "ObservableClient.h"
#include "LightConfiguration.h"

namespace winrt::Lux::implementation
{
  struct SettingsViewModel : SettingsViewModelT<SettingsViewModel>
  {
    SettingsViewModel();

    float GammaR();
    void GammaR(float value);
    float GammaG();
    void GammaG(float value);
    float GammaB();
    void GammaB(float value);
    bool IsGammaLocked();
    void IsGammaLocked(bool value);

    uint8_t BrightnessLimit();
    void BrightnessLimit(uint8_t value);

    float Saturation();
    void Saturation(float value);

    winrt::event_token PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler);
    void PropertyChanged(winrt::event_token const& token) noexcept;

  private:
    winrt::event<Windows::UI::Xaml::Data::PropertyChangedEventHandler> _propertyChanged;
    std::shared_ptr<::Lux::Observable::observable_client<::Lux::Configuration::LightConfiguration>> _client;
    std::shared_ptr<::Lux::Threading::dispatcher> _dispatcher;

    void OnFullDataReset(::Lux::Observable::observable_client<::Lux::Configuration::LightConfiguration>* sender);
    void OnSettingChanged(::Lux::Observable::observable_object<::Lux::Configuration::LightConfigurationProperty>* object, ::Lux::Configuration::LightConfigurationProperty propertyKey);

    bool _isGammaLocked = true;
  };
}

namespace winrt::Lux::factory_implementation
{
  struct SettingsViewModel : SettingsViewModelT<SettingsViewModel, implementation::SettingsViewModel>
  {
  };
}
