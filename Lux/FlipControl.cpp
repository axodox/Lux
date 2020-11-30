#include "pch.h"
#include "FlipControl.h"
#include "FlipControl.g.cpp"
#include "Events.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Input;

namespace winrt::Lux::implementation
{
  FlipControl::FlipControl()
  {
    InitializeComponent();
    Padding({ 9, 9, 9, 9 });

    _windowPointerHandler = box_value(PointerEventHandler([&](auto&, const PointerRoutedEventArgs& eventArgs) {
      auto point = eventArgs.GetCurrentPoint(*this).Position();
      if (point.X < 0 || point.Y < 0 || point.X > ActualWidth() || point.Y > ActualHeight())
      {
        IsShowingSecondary(false);
      }      
      eventArgs.Handled(IsExitClickHandled());
      }));

    ExitStoryboard().Begin();
  }

#pragma region PrimaryContent
  IInspectable FlipControl::PrimaryContent()
  {
    return GetValue(_primaryContentProperty);
  }

  void FlipControl::PrimaryContent(IInspectable const& value)
  {
    SetValue(_primaryContentProperty, value);
  }

  DependencyProperty FlipControl::PrimaryContentProperty()
  {
    return _primaryContentProperty;
  }

  DependencyProperty FlipControl::_primaryContentProperty =
    DependencyProperty::Register(
      L"PrimaryContent",
      xaml_typename<IInspectable>(),
      xaml_typename<Lux::FlipControl>(),
      nullptr);
#pragma endregion

#pragma region SecondaryContent
  IInspectable FlipControl::SecondaryContent()
  {
    return GetValue(_secondaryContentProperty);
  }

  void FlipControl::SecondaryContent(IInspectable const& value)
  {
    SetValue(_secondaryContentProperty, value);
  }

  DependencyProperty FlipControl::SecondaryContentProperty()
  {
    return _secondaryContentProperty;
  }

  DependencyProperty FlipControl::_secondaryContentProperty =
    DependencyProperty::Register(
      L"SecondaryContent",
      xaml_typename<IInspectable>(),
      xaml_typename<Lux::FlipControl>(),
      nullptr);
#pragma endregion

#pragma region IsShowingSecondary
  bool FlipControl::IsShowingSecondary()
  {
    return unbox_value<bool>(GetValue(_isShowingSecondaryProperty));
  }

  void FlipControl::IsShowingSecondary(bool value)
  {
    SetValue(_isShowingSecondaryProperty, box_value(value));
  }

  DependencyProperty FlipControl::IsShowingSecondaryProperty()
  {
    return _isShowingSecondaryProperty;
  }

  DependencyProperty FlipControl::_isShowingSecondaryProperty =
    DependencyProperty::Register(
      L"IsShowingSecondary",
      xaml_typename<bool>(),
      xaml_typename<Lux::FlipControl>(),
      PropertyMetadata{ box_value(false), &FlipControl::OnIsShowingSecondaryChanged });

  void FlipControl::OnIsShowingSecondaryChanged(DependencyObject const& sender, DependencyPropertyChangedEventArgs eventArgs)
  {
    auto control = sender.as<FlipControl>();
    if (control->IsShowingSecondary())
    {
      control->EnterStoryboard().Begin();
      
      if (control->IsExitClickHandled())
      {
        Window::Current().Content().AddHandler(UIElement::PointerReleasedEvent(), control->_windowPointerHandler, false);
      }
      else
      {
        Window::Current().Content().AddHandler(UIElement::PointerPressedEvent(), control->_windowPointerHandler, true);
      }      
    }
    else
    {
      control->ExitStoryboard().Begin();

      if (control->IsExitClickHandled())
      {
        Window::Current().Content().RemoveHandler(UIElement::PointerReleasedEvent(), control->_windowPointerHandler);
      }
      else
      {
        Window::Current().Content().RemoveHandler(UIElement::PointerPressedEvent(), control->_windowPointerHandler);
      }
    }
  }
#pragma endregion

#pragma region IsToggleVisible
  bool FlipControl::IsToggleVisible()
  {
    return unbox_value<bool>(GetValue(_isToggleVisibleProperty));
  }

  void FlipControl::IsToggleVisible(bool value)
  {
    SetValue(_isToggleVisibleProperty, box_value(value));
  }

  DependencyProperty FlipControl::IsToggleVisibleProperty()
  {
    return _isToggleVisibleProperty;
  }

  DependencyProperty FlipControl::_isToggleVisibleProperty =
    DependencyProperty::Register(
      L"IsToggleVisible",
      xaml_typename<bool>(),
      xaml_typename<Lux::FlipControl>(),
      PropertyMetadata{ box_value(true) });
#pragma endregion
    
#pragma region IsExitClickHandled
  bool FlipControl::IsExitClickHandled()
  {
    return unbox_value<bool>(GetValue(_isExitClickHandledProperty));
  }

  void FlipControl::IsExitClickHandled(bool value)
  {
    SetValue(_isExitClickHandledProperty, box_value(value));
  }

  DependencyProperty FlipControl::IsExitClickHandledProperty()
  {
    return _isExitClickHandledProperty;
  }

  DependencyProperty FlipControl::_isExitClickHandledProperty =
    DependencyProperty::Register(
      L"IsExitClickHandledProperty",
      xaml_typename<bool>(),
      xaml_typename<Lux::FlipControl>(),
      PropertyMetadata{ box_value(false) });
#pragma endregion
}
