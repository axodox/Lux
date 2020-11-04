#pragma once

#include "MainPage.g.h"

namespace winrt::Lux::implementation
{
  struct MainPage : MainPageT<MainPage>
  {
    MainPage();
  };
}

namespace winrt::Lux::factory_implementation
{
  struct MainPage : MainPageT<MainPage, implementation::MainPage>
  {
  };
}
