#pragma once
#include "SourcesViewModel.g.h"

#include "ObservableClient.h"
#include "LightConfiguration.h"

namespace winrt::Lux::implementation
{
  struct SourcesViewModel : SourcesViewModelT<SourcesViewModel>
  {
    SourcesViewModel();

    SourceKind Source();
    void Source(SourceKind value);

    Windows::UI::Color StaticSourceColor();
    void StaticSourceColor(Windows::UI::Color value);

    uint8_t RainbowSourceSpatialFrequency();
    void RainbowSourceSpatialFrequency(uint8_t value);
    float RainbowSourceAngularVelocity();
    void RainbowSourceAngularVelocity(float value);

    float DesktopSourceTemporalAveraging();
    void DesktopSourceTemporalAveraging(float value);
    float DesktopSourceSampleSize();
    void DesktopSourceSampleSize(float value);
        
    winrt::event_token PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler);
    void PropertyChanged(winrt::event_token const& token);

  private:
    winrt::event<Windows::UI::Xaml::Data::PropertyChangedEventHandler> _propertyChanged;
    std::shared_ptr<::Lux::Observable::observable_client<::Lux::Configuration::LightConfiguration>> _client;
    std::shared_ptr<::Lux::Threading::dispatcher> _dispatcher;

    void OnFullDataReset(::Lux::Observable::observable_client<::Lux::Configuration::LightConfiguration>* sender);
    void OnSettingChanged(::Lux::Observable::observable_object<::Lux::Configuration::LightConfigurationProperty>* object, ::Lux::Configuration::LightConfigurationProperty propertyKey);
  };
}

namespace winrt::Lux::factory_implementation
{
  struct SourcesViewModel : SourcesViewModelT<SourcesViewModel, implementation::SourcesViewModel>
  {
  };
}
