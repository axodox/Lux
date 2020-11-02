#include "pch.h"
#include "EnumNameMapping.h"
#include "Text.h"

namespace Lux::Text
{
  std::vector<std::wstring> split_enum_names(const char* text)
  {
    std::vector<std::wstring> values;

    auto keys = split(to_wstring(text), ',');
    values.reserve(keys.size());

    for (auto& key : keys)
    {
      trim(key);
      key[0] = (char)tolower(key[0]);
      values.push_back(key);
    }

    return values;
  }
}