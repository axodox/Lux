#pragma once
#include "BooleanToVisibilityConverter.g.h"

namespace winrt::Lux::implementation
{
  struct BooleanToVisibilityConverter : BooleanToVisibilityConverterT<BooleanToVisibilityConverter>
  {
    BooleanToVisibilityConverter() = default;

    Windows::Foundation::IInspectable Convert(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, hstring const& language);
    Windows::Foundation::IInspectable ConvertBack(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, hstring const& language);
  };
}

namespace winrt::Lux::factory_implementation
{
  struct BooleanToVisibilityConverter : BooleanToVisibilityConverterT<BooleanToVisibilityConverter, implementation::BooleanToVisibilityConverter>
  {
  };
}
