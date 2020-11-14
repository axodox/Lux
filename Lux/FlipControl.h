#pragma once
#include "FlipControl.g.h"
#include "BooleanToVisibilityConverter.h"

namespace winrt::Lux::implementation
{
  struct FlipControl : FlipControlT<FlipControl>
  {
    FlipControl();

    Windows::Foundation::IInspectable PrimaryContent();
    void PrimaryContent(Windows::Foundation::IInspectable const& value);
    static Windows::UI::Xaml::DependencyProperty PrimaryContentProperty();

    Windows::Foundation::IInspectable SecondaryContent();
    void SecondaryContent(Windows::Foundation::IInspectable const& value);
    static Windows::UI::Xaml::DependencyProperty SecondaryContentProperty();

    bool IsShowingSecondary();
    void IsShowingSecondary(bool value);
    static Windows::UI::Xaml::DependencyProperty IsShowingSecondaryProperty();

    bool IsToggleVisible();
    void IsToggleVisible(bool value);
    static Windows::UI::Xaml::DependencyProperty IsToggleVisibleProperty();

    bool IsExitClickHandled();
    void IsExitClickHandled(bool value);
    static Windows::UI::Xaml::DependencyProperty IsExitClickHandledProperty();

  private:
    static Windows::UI::Xaml::DependencyProperty _primaryContentProperty;
    static Windows::UI::Xaml::DependencyProperty _secondaryContentProperty;
    static Windows::UI::Xaml::DependencyProperty _isShowingSecondaryProperty;
    static Windows::UI::Xaml::DependencyProperty _isToggleVisibleProperty;
    static Windows::UI::Xaml::DependencyProperty _isExitClickHandledProperty;

    IInspectable _windowPointerHandler;

    static void OnIsShowingSecondaryChanged(Windows::UI::Xaml::DependencyObject const& sender, Windows::UI::Xaml::DependencyPropertyChangedEventArgs eventArgs);
  };
}

namespace winrt::Lux::factory_implementation
{
  struct FlipControl : FlipControlT<FlipControl, implementation::FlipControl>
  {
  };
}
