#pragma once

#include "MainPage.g.h"

namespace winrt::Lux_Configurator::implementation
{
  struct MainPage : MainPageT<MainPage>
  {
    MainPage();

    int32_t MyProperty();
    void MyProperty(int32_t value);

    void ClickHandler(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::RoutedEventArgs const& args);

  private:
    fire_and_forget Load();
  };
}

namespace winrt::Lux_Configurator::factory_implementation
{
  struct MainPage : MainPageT<MainPage, implementation::MainPage>
  {
  };
}
