#pragma once
#include "shared_pch.h"

namespace Lux::Traits
{
  template<template<typename...> class, typename...>
  struct is_instantiation_of : public std::false_type {};

  template<template<typename...> class U, typename... T>
  struct is_instantiation_of<U, U<T...>> : public std::true_type {};

  struct test_has_actual_types {
    template<class T>
    static std::true_type test(decltype(&T::actual_types));

    template<class>
    static std::false_type test(...);
  };

  template<class T>
  struct has_actual_types : decltype(test_has_actual_types::test<T>(nullptr))
  {};
}