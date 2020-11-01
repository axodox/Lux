#pragma once

#include "MainPage.g.h"

namespace winrt::Lux::Settings::implementation
{
  struct MainPage : MainPageT<MainPage>
  {
    MainPage();

    int32_t MyProperty();
    void MyProperty(int32_t value);

    fire_and_forget ClickHandler(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);
  };
}

namespace winrt::Lux::Settings::factory_implementation
{
  struct MainPage : MainPageT<MainPage, implementation::MainPage>
  {
  };
}
