#pragma once

#include "winrt/Windows.UI.Xaml.h"
#include "winrt/Windows.UI.Xaml.Markup.h"
#include "winrt/Windows.UI.Xaml.Interop.h"
#include "winrt/Windows.UI.Xaml.Controls.Primitives.h"
#include "SettingsView.g.h"

#include "BooleanToStringSwitchConverter.h"

namespace winrt::Lux::implementation
{
  struct SettingsView : SettingsViewT<SettingsView>
  {
    SettingsView();

    SettingsViewModel ViewModel();

  private:
    SettingsViewModel _viewModel{};
  };
}

namespace winrt::Lux::factory_implementation
{
  struct SettingsView : SettingsViewT<SettingsView, implementation::SettingsView>
  {
  };
}
