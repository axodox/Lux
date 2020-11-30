#pragma once
#include "shared_pch.h"

namespace Lux::BitwiseOperations
{
  template<typename T>
  inline constexpr T bitwise_negate(const T a)
  {
    return (T)(~static_cast<std::underlying_type_t<T>>(a));
  }

  template<
    typename T,
    typename U,
    typename V = std::underlying_type_t<T>,
    typename = std::enable_if<std::is_same<std::underlying_type_t<T>, std::underlying_type_t<U>>::value>>
    inline constexpr V bitwise_or(const T a, const U b)
  {
    return (V)(static_cast<V>(a) | static_cast<V>(b));
  }

  template<typename T>
  inline constexpr T bitwise_or(const T a, const T b)
  {
    return (T)(static_cast<std::underlying_type_t<T>>(a) | static_cast<std::underlying_type_t<T>>(b));
  }

  template<typename THead, typename... TTail, class = std::enable_if_t<std::conjunction<std::is_same<THead, TTail>...>::value, void>>
  inline constexpr THead bitwise_or(const THead head, const TTail ... tail)
  {
    return (THead)(static_cast<std::underlying_type_t<THead>>(head) | ... | static_cast<std::underlying_type_t<TTail>>(tail));
  }

  template<typename T>
  inline constexpr T bitwise_and(const T a, const T b)
  {
    return (T)(static_cast<std::underlying_type_t<T>>(a) & static_cast<std::underlying_type_t<T>>(b));
  }

  template<typename T>
  inline constexpr bool has_flag(const T a, const T b)
  {
    return bitwise_and(a, b) == b;
  }

  template<typename T>
  inline constexpr bool has_any_flag(const T a, const T b)
  {
    return bitwise_and(a, b) != (T)0;
  }

  template<typename T>
  inline void set_flag(T& a, const T b, const bool value)
  {
    if (value)
    {
      a = bitwise_or(a, b);
    }
    else
    {
      a = bitwise_and(a, bitwise_negate(b));
    }
  }

  template <typename T, size_t C>
  bool are_equal(const std::array<T, C>& a, const std::array<T, C>& b)
  {
    return memcmp(a.data(), b.data(), sizeof(T) * C) == 0;
  }

  template <typename T>
  bool are_equal(const T& a, const T& b)
  {
    return memcmp(&a, &b, sizeof(T)) == 0;
  }

  template <>
  inline bool are_equal<std::string>(const std::string& a, const std::string& b)
  {
    return a.compare(b) == 0;
  }

  template <>
  inline bool are_equal<std::wstring>(const std::wstring& a, const std::wstring& b)
  {
    return a.compare(b) == 0;
  }

  template <typename T>
  bool are_equal(const std::vector<T>& a, const std::vector<T>& b)
  {
    if (a.size() != b.size()) return false;
    return memcmp(a.data(), b.data(), sizeof(T) * a.size()) == 0;
  }

  template <typename T>
  bool is_default(const T& value)
  {
    return are_equal<T>(value, T{});
  }
}