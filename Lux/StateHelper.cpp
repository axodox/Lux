#include "pch.h"
#include "StateHelper.h"
#include "StateHelper.g.cpp"

using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Controls;

namespace winrt::Lux::implementation
{
  Windows::UI::Xaml::DependencyProperty StateHelper::StateProperty()
  {
    return _stateProperty;
  }

  void StateHelper::OnStateChanged(Windows::UI::Xaml::DependencyObject const& target, Windows::UI::Xaml::DependencyPropertyChangedEventArgs const& args)
  {
    auto state = unbox_value_or<hstring>(args.NewValue(), L"");
    const auto& control = target.try_as<Control>();

    if (!state.empty() && control)
    {
      VisualStateManager::GoToState(control, state, true);
    }
  }

  hstring StateHelper::GetState(Windows::UI::Xaml::UIElement target)
  {
    return unbox_value_or<hstring>(target.GetValue(_stateProperty), L"");
  }

  void StateHelper::SetState(Windows::UI::Xaml::UIElement target, hstring value)
  {
    target.SetValue(_stateProperty, box_value(value));
  }

  Windows::UI::Xaml::DependencyProperty StateHelper::_stateProperty =
    Windows::UI::Xaml::DependencyProperty::RegisterAttached(
      L"State",
      winrt::xaml_typename<hstring>(),
      winrt::xaml_typename<Lux::StateHelper>(),
      Windows::UI::Xaml::PropertyMetadata{ nullptr, Windows::UI::Xaml::PropertyChangedCallback{ &StateHelper::OnStateChanged } });
}
