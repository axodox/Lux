#pragma once
#include "StateHelper.g.h"

namespace winrt::Lux::implementation
{
  struct StateHelper : StateHelperT<StateHelper>
  {
    StateHelper() = default;

    static Windows::UI::Xaml::DependencyProperty StateProperty();
    static void OnStateChanged(Windows::UI::Xaml::DependencyObject const& target, Windows::UI::Xaml::DependencyPropertyChangedEventArgs const& args);

    static hstring GetState(Windows::UI::Xaml::UIElement target);
    static void SetState(Windows::UI::Xaml::UIElement target, hstring value);

  private:
    static Windows::UI::Xaml::DependencyProperty _stateProperty;
  };
}
namespace winrt::Lux::factory_implementation
{
  struct StateHelper : StateHelperT<StateHelper, implementation::StateHelper>
  {
  };
}
