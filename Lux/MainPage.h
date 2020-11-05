#pragma once

#include "MainPage.g.h"

namespace winrt::Lux::implementation
{
  struct MainPage : MainPageT<MainPage>
  {
    MainPage();

  private:
    Windows::ApplicationModel::Core::CoreApplicationViewTitleBar::LayoutMetricsChanged_revoker _titleBarLayoutMetricsChangedRevoker;
    Windows::UI::Core::CoreWindow::SizeChanged_revoker _windowSizeChangedRevoker;
    void UpdateTitleBarLayout(const Windows::ApplicationModel::Core::CoreApplicationViewTitleBar& titleBar);
  };
}

namespace winrt::Lux::factory_implementation
{
  struct MainPage : MainPageT<MainPage, implementation::MainPage>
  {
  };
}
