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

    event_token PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& value);
    void PropertyChanged(winrt::event_token const& token);

  private:
    static Windows::UI::Xaml::DependencyProperty _primaryContentProperty;
    static Windows::UI::Xaml::DependencyProperty _secondaryContentProperty;

    event<Windows::UI::Xaml::Data::PropertyChangedEventHandler> _propertyChanged;
    bool _isShowingSecondary = false;

    IInspectable _windowPointerPressedHandler;
  };
}

namespace winrt::Lux::factory_implementation
{
  struct FlipControl : FlipControlT<FlipControl, implementation::FlipControl>
  {
  };
}
