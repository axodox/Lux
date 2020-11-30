#include "pch.h"
#include "MainPage.h"
#include "MainPage.g.cpp"
#include "DeviceSettings.h"
#include "DependencyConfiguration.h"
#include "ThreadName.h"

using namespace ::Lux;
using namespace ::Lux::Events;
using namespace ::Lux::Observable;
using namespace ::Lux::Graphics;
using namespace ::Lux::Configuration;
using namespace ::Lux::Threading;
using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::UI;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::ViewManagement;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::Data::Json;

namespace winrt::Lux::implementation
{
  MainPage::MainPage() :
    _client(DependencyConfiguration::Instance().resolve<observable_client<LightConfiguration>>())
  {
    InitializeComponent();
    InitializeView();

    _client->is_connected_changed(no_revoke, member_func(this, &MainPage::OnClientConnectedChanged));
    _client->full_data_reset(no_revoke, member_func(this, &MainPage::OnFullDataReset));

    _client->root()->property_changed(no_revoke, member_func(this, &MainPage::OnSettingChanged));
    _client->open();
  }

  bool MainPage::IsConnected()
  {
    return _client->is_connected();
  }

  hstring MainPage::ConnectionState()
  {
    if (_client->is_connected())
    {
      return _client->root()->IsConnected ? L"Connected" : L"Disconnected";
    }
    else
    {
      return L"Idle";
    }
  }

  void MainPage::InitializeView()
  {
    set_thread_name(L"* UI thread");

    auto currentView = ApplicationView::GetForCurrentView();
    currentView.PreferredLaunchViewSize({ 1024, 512 });
    currentView.PreferredLaunchWindowingMode(ApplicationViewWindowingMode::PreferredLaunchViewSize);

    const auto& coreTitleBar = CoreApplication::GetCurrentView().TitleBar();
    coreTitleBar.ExtendViewIntoTitleBar(true);

    _titleBarLayoutMetricsChangedRevoker = coreTitleBar.LayoutMetricsChanged(auto_revoke, [this](const CoreApplicationViewTitleBar& sender, const IInspectable&) {
      UpdateTitleBarLayout(sender);
      });
    UpdateTitleBarLayout(coreTitleBar);
    Window::Current().SetTitleBar(AppTitleBar());

    const auto& viewTitleBar = ApplicationView::GetForCurrentView().TitleBar();
    viewTitleBar.ButtonBackgroundColor(Colors::Transparent());
    viewTitleBar.ButtonForegroundColor(Colors::White());
    viewTitleBar.ButtonInactiveBackgroundColor(Colors::Transparent());
    viewTitleBar.ButtonInactiveForegroundColor(Colors::White());
  }

  void MainPage::UpdateTitleBarLayout(const CoreApplicationViewTitleBar& titleBar)
  {
    AppTitleBar().Height(titleBar.Height());

    TitleBarLeftPaddingColumn().Width({ titleBar.SystemOverlayLeftInset() });
    TitleBarRightPaddingColumn().Width({ titleBar.SystemOverlayRightInset() });
  }

  void MainPage::OnClientConnectedChanged(observable_client<LightConfiguration>* /*sender*/)
  {
    Dispatcher().RunAsync({}, [&] {
      _propertyChanged(*this, PropertyChangedEventArgs(L"IsConnected"));
      _propertyChanged(*this, PropertyChangedEventArgs(L"ConnectionState"));
      }).get();
  }

  void MainPage::OnFullDataReset(::Lux::Observable::observable_client<::Lux::Configuration::LightConfiguration>* /*sender*/)
  {
    Dispatcher().RunAsync({}, [&] {
      _propertyChanged(*this, PropertyChangedEventArgs(L""));
      });
  }

  void MainPage::OnSettingChanged(observable_object<LightConfigurationProperty>* object, LightConfigurationProperty propertyKey)
  {
    switch (propertyKey)
    {
    case LightConfigurationProperty::IsConnected:
      _propertyChanged(*this, PropertyChangedEventArgs(L"ConnectionState"));
      break;
    case LightConfigurationProperty::Brightness:
      _propertyChanged(*this, PropertyChangedEventArgs(L"Brightness"));
      break;
    }
  }

  fire_and_forget MainPage::ConfigureDevice()
  {
    FileOpenPicker picker{};
    picker.ViewMode(PickerViewMode::List);
    picker.SuggestedStartLocation(PickerLocationId::DocumentsLibrary);
    picker.FileTypeFilter().Append(L".json");
    picker.CommitButtonText(L"Open configuration");
    picker.SettingsIdentifier(L"device_configuration");

    auto file = co_await picker.PickSingleFileAsync();
    if (!file) co_return;

    ContentDialog messageDialog{};
    messageDialog.Title(box_value(L"Lux configuration importer"));
    messageDialog.CloseButtonText(L"OK");

    try
    {
      auto text = co_await FileIO::ReadTextAsync(file);

      auto json = JsonValue::Parse(text);
      DeviceSettings settings{ json };

      auto ledCount = settings.Layout.LedCount();
      if (ledCount > 0u)
      {
        _client->root()->Device.value(std::move(settings));
        messageDialog.Content(box_value(L"Successfully loaded configuration from " + file.Path() + L"."));
      }
      else
      {
        messageDialog.Content(box_value(L"The configuration at path " + file.Path() + L" contains no LEDs."));
      }
    }
    catch (const hresult_error& e)
    {
      messageDialog.Content(box_value(L"Failed to load configuration " + file.Path() + L". Reason: " + e.message()));
    }
    catch (...)
    {
      messageDialog.Content(box_value(L"Failed to load configuration " + file.Path() + L". Unknown error."));
    }

    if (messageDialog)
    {
      co_await messageDialog.ShowAsync();
    }
  }

  uint8_t MainPage::Brightness()
  {
    return _client->root()->Brightness;
  }

  void MainPage::Brightness(uint8_t value)
  {
    if (_client->root()->Brightness == value) return;
    _client->root()->Brightness = value;
  }

  winrt::event_token MainPage::PropertyChanged(PropertyChangedEventHandler const& value)
  {
    return _propertyChanged.add(value);
  }

  void MainPage::PropertyChanged(event_token const& token)
  {
    _propertyChanged.remove(token);
  }

  bool MainPage::IsShowingSettings()
  {
    return _isShowingSettings;
  }

  void MainPage::IsShowingSettings(bool value)
  {
    _isShowingSettings = value;
    _propertyChanged(*this, PropertyChangedEventArgs(L"IsShowingSettings"));
  }
}
