#include "pch.h"
#include "BooleanToStringSwitchConverter.h"
#include "BooleanToStringSwitchConverter.g.cpp"

using namespace std;
using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Interop;

namespace winrt::Lux::implementation
{
  IInspectable BooleanToStringSwitchConverter::Convert(IInspectable const& value, TypeName const& /*targetType*/, IInspectable const& parameter, hstring const& /*language*/)
  {
    auto strings = std::wstring(unbox_value_or(parameter, L"true|false"));
    auto split = wcschr(strings.c_str(), L'|');

    if (split)
    {
      auto onText = strings.substr(0, split - strings.data());
      auto offText = strings.substr(split - strings.data() + 1);
      return box_value(winrt::hstring(unbox_value_or(value, false) ? onText : offText));
    }
    else
    {
      return DependencyProperty::UnsetValue();
    }
  }

  IInspectable BooleanToStringSwitchConverter::ConvertBack(IInspectable const& /*value*/, TypeName const& /*targetType*/, IInspectable const& /*parameter*/, hstring const& /*language*/)
  {
    throw hresult_not_implemented();
  }
}
