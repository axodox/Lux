#pragma once
#include "Text.h"
#include "TypeRegistry.h"
#include "LifetimeExecutor.h"

namespace Lux::Text
{
  template<typename T>
  struct enum_value_name_mapping
  {
    inline static std::vector<std::wstring> values;

    static std::wstring name(T value)
    {
      if (values.empty())
      {
        return std::to_wstring(static_cast<std::underlying_type_t<T>>(value));
      }
      else
      {
        return values.at(static_cast<std::underlying_type_t<T>>(value));
      }
    }

    static T value(const std::wstring& name)
    {
      if (values.empty())
      {
        return (T)std::stoi(name);
      }
      else
      {
        auto it = std::find(values.begin(), values.end(), name);
        if (it != values.end())
        {
          return (T)std::distance(values.begin(), it);
        }
        else
        {
          return (T)-1;
        }
      }
    }
  };

  std::vector<std::wstring> split_enum_names(const char* text);

#define named_enum(enum_t, ...) enum class enum_t { __VA_ARGS__ }; \
  namespace { \
    inline void init_enum_##enum_t() { Lux::Text::enum_value_name_mapping<enum_t>::values = Lux::Text::split_enum_names(#__VA_ARGS__); } \
    inline Threading::lifetime_executor<init_enum_##enum_t, nullptr> run_enum_##enum_t; \
  }

#define named_enum_underlying(enum_t, underlying_t, ...) enum class enum_t : underlying_t { __VA_ARGS__ }; \
  namespace { \
    inline void init_enum_##enum_t() { Lux::Text::enum_value_name_mapping<enum_t>::values = Lux::Text::split_enum_names(#__VA_ARGS__); } \
    inline Threading::lifetime_executor<init_enum_##enum_t, nullptr> run_enum_##enum_t; \
  }

#define named_enum_typeid(enum_t, ...) named_enum_underlying(enum_t, Serialization::type_id_t, __VA_ARGS__)
}