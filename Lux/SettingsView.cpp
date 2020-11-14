#include "pch.h"
#include "SettingsView.h"
#if __has_include("SettingsView.g.cpp")
#include "SettingsView.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::Lux::implementation
{
  SettingsView::SettingsView()
  {
    InitializeComponent();
  }

  SettingsViewModel SettingsView::ViewModel()
  {
    return _viewModel;
  }
}
