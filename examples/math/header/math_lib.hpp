#pragma once

#include "library.hpp"

#include "gain.hpp"
#include "mathop.hpp"
#include "sink.hpp"
#include "source.hpp"
#include "type_convertor.hpp"
#include "unit_delay.hpp"

#include <cstdint>
#include <sys/types.h>
#include <type_traits>

namespace mbd
{
namespace impl
{

template <typename T>
lib get_math_lib(const std::string &type_name)
{

  lib math_lib("MathLib - " + type_name);

  math_lib.register_model<const_source<T>>("const_src");
  math_lib.register_model<liniar_source<T>>("lin_src");

  math_lib.register_model<sink<T>>("sink");
  math_lib.register_model<gain<T>>("gain");

  math_lib.register_model<add<T>>("sum");
  math_lib.register_model<subtract<T>>("diff");
  math_lib.register_model<div<T>>("div");

  constexpr bool is_float = std::is_same<T, float>::value;
  constexpr bool is_double = std::is_same<T, double>::value;

  if constexpr (!(is_float || is_double))
    math_lib.register_model<mod<T>>("mod");

  math_lib.register_model<multiply<T>>("multiply");

  math_lib.register_model<type_convertor<float, T>>("float_to");
  math_lib.register_model<type_convertor<double, T>>("double_to");
  math_lib.register_model<type_convertor<int, T>>("int_to");
  math_lib.register_model<type_convertor<uint, T>>("uint_to");

  math_lib.register_model<unit_delay<T>>("delay");

  return math_lib;
}

} // namespace impl
} // namespace mbd