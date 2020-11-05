#include "pch.h"
#include "MainPage.h"
#include "MainPage.g.cpp"

using namespace winrt;
using namespace Windows::Foundation;
using namespace winrt::Windows::ApplicationModel::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::ViewManagement;

namespace winrt::Lux::implementation
{
  MainPage::MainPage()
  {
    InitializeComponent();
    
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
    viewTitleBar.ButtonBackgroundColor(Windows::UI::Colors::Transparent());
    viewTitleBar.ButtonForegroundColor(Windows::UI::Colors::White());
    viewTitleBar.ButtonInactiveBackgroundColor(Windows::UI::Colors::Transparent());
    viewTitleBar.ButtonInactiveForegroundColor(Windows::UI::Colors::White());
  }

  void MainPage::UpdateTitleBarLayout(const Windows::ApplicationModel::Core::CoreApplicationViewTitleBar& titleBar)
  {
    AppTitleBar().Height(titleBar.Height());

    TitleBarLeftPaddingColumn().Width({ titleBar.SystemOverlayLeftInset() });
    TitleBarRightPaddingColumn().Width({ titleBar.SystemOverlayRightInset() });
  }
}
