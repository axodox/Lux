#pragma once
#include "BooleanToStringSwitchConverter.g.h"

namespace winrt::Lux::implementation
{
  struct BooleanToStringSwitchConverter : BooleanToStringSwitchConverterT<BooleanToStringSwitchConverter>
  {
    BooleanToStringSwitchConverter() = default;

    Windows::Foundation::IInspectable Convert(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, hstring const& language);
    Windows::Foundation::IInspectable ConvertBack(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, hstring const& language);
  };
}

namespace winrt::Lux::factory_implementation
{
  struct BooleanToStringSwitchConverter : BooleanToStringSwitchConverterT<BooleanToStringSwitchConverter, implementation::BooleanToStringSwitchConverter>
  {
  };
}
