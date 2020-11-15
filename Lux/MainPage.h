#pragma once
#include "MainPage.g.h"

#include "ObservableClient.h"
#include "LightConfiguration.h"

namespace winrt::Lux::implementation
{
  struct MainPage : MainPageT<MainPage>
  {
    MainPage();

    bool IsConnected();
    hstring ConnectionState();

    fire_and_forget ConfigureDevice();

    uint8_t Brightness();
    void Brightness(uint8_t value);

    bool IsShowingSettings();
    void IsShowingSettings(bool value);

    winrt::event_token PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& value);
    void PropertyChanged(winrt::event_token const& token);

  private:
    winrt::event<Windows::UI::Xaml::Data::PropertyChangedEventHandler> _propertyChanged;
    Windows::ApplicationModel::Core::CoreApplicationViewTitleBar::LayoutMetricsChanged_revoker _titleBarLayoutMetricsChangedRevoker;
    Windows::UI::Core::CoreWindow::SizeChanged_revoker _windowSizeChangedRevoker;
    bool _isShowingSettings = false;
    void InitializeView();
    void UpdateTitleBarLayout(const Windows::ApplicationModel::Core::CoreApplicationViewTitleBar& titleBar);

    std::shared_ptr<::Lux::Observable::observable_client<::Lux::Configuration::LightConfiguration>> _client;
    void OnClientConnectedChanged(::Lux::Observable::observable_client<::Lux::Configuration::LightConfiguration>* sender);
    void OnFullDataReset(::Lux::Observable::observable_client<::Lux::Configuration::LightConfiguration>* sender);

    void OnSettingChanged(::Lux::Observable::observable_object<::Lux::Configuration::LightConfigurationProperty>* object, ::Lux::Configuration::LightConfigurationProperty propertyKey);
  };
}

namespace winrt::Lux::factory_implementation
{
  struct MainPage : MainPageT<MainPage, implementation::MainPage>
  {
  };
}
