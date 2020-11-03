#include "pch.h"
#include "MainPage.h"
#include "MainPage.g.cpp"

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::Lux::Settings::implementation
{
  MainPage::MainPage()
  {
    InitializeComponent();
  }

  int32_t MainPage::MyProperty()
  {
    throw hresult_not_implemented();
  }

  void MainPage::MyProperty(int32_t /* value */)
  {
    throw hresult_not_implemented();
  }

  fire_and_forget MainPage::ClickHandler(IInspectable const&, RoutedEventArgs const&)
  {
    com_ptr<IDXGIOutput6> out;
    out->DuplicateOutput1();
    myButton().Content(box_value(L"Clicked"));
    co_await Windows::ApplicationModel::FullTrustProcessLauncher::LaunchFullTrustProcessForCurrentAppAsync();
  }
}
