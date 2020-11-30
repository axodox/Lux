#pragma once
#include "shared_pch.h"

namespace Lux::Text
{
  void trim_left(std::wstring& str);
  void trim_right(std::wstring& str);
  void trim(std::wstring& str);

  template <typename T, typename = std::enable_if_t<std::is_convertible_v<T, std::string_view>>>
  std::wstring to_wstring(T const& value)
  {
    std::string_view const view(value);
    const auto expectedSize = MultiByteToWideChar(CP_UTF8, 0, view.data(), static_cast<int32_t>(view.size()), nullptr, 0);

    if (expectedSize == 0)
    {
      return {};
    }

    std::wstring result(expectedSize, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, view.data(), static_cast<int32_t>(view.size()), result.data(), expectedSize);
    return result;
  }

  std::vector<std::wstring> split(const std::wstring_view& text, wchar_t delimiter);
}