#pragma once

#include "library.hpp"

#include "gain.hpp"
#include "mathop.hpp"
#include "sink.hpp"
#include "source.hpp"
#include "type_convertor.hpp"
#include "unit_delay.hpp"

#include <sys/types.h>
#include <type_traits>

using namespace mbd;

namespace tst
{

template <typename T>
mbd::lib get_math_lib(const std::string &type_name)
{

  mbd::lib math_lib("MathLib - " + type_name);

  math_lib.register_model<const_source<T>>("const_src");
  math_lib.register_model<linear_source<T>>("lin_src");

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

  math_lib.register_model<type_convertor<T, T>>("double_to");

  math_lib.register_model<unit_delay<T>>("delay");

  return math_lib;
}

} 