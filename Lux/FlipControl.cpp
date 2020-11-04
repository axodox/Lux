#include "pch.h"
#include "FlipControl.h"
#include "FlipControl.g.cpp"

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::Lux::implementation
{
  FlipControl::FlipControl()
  {
    InitializeComponent();

    _windowPointerPressedHandler = box_value(winrt::Windows::UI::Xaml::Input::PointerEventHandler([&](auto&, const winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs& eventArgs) {
      auto point = eventArgs.GetCurrentPoint(*this).Position();
      if (point.X < 0 || point.Y < 0 || point.X > ActualWidth() || point.Y > ActualHeight())
      {
        IsShowingSecondary(false);
      }
      }));

    ExitStoryboard().Begin();
  }

  Windows::Foundation::IInspectable FlipControl::PrimaryContent()
  {
    return GetValue(_primaryContentProperty);
  }

  void FlipControl::PrimaryContent(Windows::Foundation::IInspectable const& value)
  {
    SetValue(_primaryContentProperty, value);
  }

  Windows::UI::Xaml::DependencyProperty FlipControl::PrimaryContentProperty()
  {
    return _primaryContentProperty;
  }

  Windows::UI::Xaml::DependencyProperty FlipControl::_primaryContentProperty =
    Windows::UI::Xaml::DependencyProperty::Register(
      L"PrimaryContent",
      xaml_typename<Windows::Foundation::IInspectable>(),
      xaml_typename<Lux::FlipControl>(),
      nullptr);

  Windows::Foundation::IInspectable FlipControl::SecondaryContent()
  {
    return GetValue(_secondaryContentProperty);
  }

  void FlipControl::SecondaryContent(Windows::Foundation::IInspectable const& value)
  {
    SetValue(_secondaryContentProperty, value);
  }

  Windows::UI::Xaml::DependencyProperty FlipControl::SecondaryContentProperty()
  {
    return _secondaryContentProperty;
  }

  bool FlipControl::IsShowingSecondary()
  {
    return _isShowingSecondary;
  }

  void FlipControl::IsShowingSecondary(bool value)
  {
    _isShowingSecondary = value;

    if (value)
    {
      EnterStoryboard().Begin();
      Window::Current().Content().AddHandler(UIElement::PointerPressedEvent(), _windowPointerPressedHandler, true);
    }
    else
    {
      ExitStoryboard().Begin();
      Window::Current().Content().RemoveHandler(UIElement::PointerPressedEvent(), _windowPointerPressedHandler);
    }

    _propertyChanged(*this, PropertyChangedEventArgs(L"IsShowingSecondary"));
  }

  Windows::UI::Xaml::DependencyProperty FlipControl::_secondaryContentProperty =
    Windows::UI::Xaml::DependencyProperty::Register(
      L"SecondaryContent",
      xaml_typename<Windows::Foundation::IInspectable>(),
      xaml_typename<Lux::FlipControl>(),
      nullptr);

  winrt::event_token FlipControl::PropertyChanged(Data::PropertyChangedEventHandler const& value)
  {
    return _propertyChanged.add(value);
  }

  void FlipControl::PropertyChanged(winrt::event_token const& token)
  {
    _propertyChanged.remove(token);
  }
}
