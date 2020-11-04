#include "pch.h"
#include "BooleanToVisibilityConverter.h"
#include "BooleanToVisibilityConverter.g.cpp"

using namespace winrt::Windows::UI::Xaml;

namespace winrt::Lux::implementation
{
  Windows::Foundation::IInspectable BooleanToVisibilityConverter::Convert(Windows::Foundation::IInspectable const& value, Windows::UI::Xaml::Interop::TypeName const& /*targetType*/, Windows::Foundation::IInspectable const& /*parameter*/, hstring const& /*language*/)
  {
    auto input = unbox_value_or<bool>(value, false);

    return box_value(input ? Visibility::Visible : Visibility::Collapsed);
  }

  Windows::Foundation::IInspectable BooleanToVisibilityConverter::ConvertBack(Windows::Foundation::IInspectable const& /*value*/, Windows::UI::Xaml::Interop::TypeName const& /*targetType*/, Windows::Foundation::IInspectable const& /*parameter*/, hstring const& /*language*/)
  {
    throw hresult_not_implemented();
  }
}
