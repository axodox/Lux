#include "pch.h"
#include "MainPage.h"
#include "MainPage.g.cpp"
#include "DeviceSettings.h"
#include "DependencyConfiguration.h"

using namespace ::Lux;
using namespace ::Lux::Observable;
using namespace ::Lux::Configuration;
using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::UI;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::ViewManagement;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::Data::Json;

namespace winrt::Lux::implementation
{
  MainPage::MainPage() :
    _client(DependencyConfiguration::Instance().resolve<observable_client<LightConfiguration>>()),
    _clientConnectedChanged(_client->is_connected_changed([strong_this{ get_strong() }](auto) { strong_this->OnClientConnectedChanged(); }))
  {
    InitializeComponent();    
    InitializeView();
  }

  bool MainPage::IsConnected()
  {
    return _client->is_connected();
  }

  hstring MainPage::ConnectionState()
  {
    if (_client->is_connected())
    {
      return L"Connected";
    }
    else
    {
      return L"Pending";
    }
  }

  void MainPage::InitializeView()
  {
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

  void MainPage::UpdateTitleBarLayout(const Windows::ApplicationModel::Core::CoreApplicationViewTitleBar& titleBar)
  {
    AppTitleBar().Height(titleBar.Height());

    TitleBarLeftPaddingColumn().Width({ titleBar.SystemOverlayLeftInset() });
    TitleBarRightPaddingColumn().Width({ titleBar.SystemOverlayRightInset() });
  }

  void MainPage::OnClientConnectedChanged()
  {
    Dispatcher().RunAsync({}, [&] {
      _propertyChanged(*this, PropertyChangedEventArgs(L"ConnectionState"));
    }).get();
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

      auto ledCount = settings.LedCount();
      if (ledCount > 0u)
      {
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
  
  winrt::event_token MainPage::PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& value)
  {
    return _propertyChanged.add(value);
  }

  void MainPage::PropertyChanged(winrt::event_token const& token)
  {
    _propertyChanged.remove(token);
  }
}
