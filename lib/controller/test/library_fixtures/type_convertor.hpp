#pragma once

#include "model.hpp"

#include <limits>
#include <type_traits>
#include <typeinfo>

using namespace mbd;
namespace tst
{

template <typename IN, typename OUT>
class type_convertor : public model
{

public:
  type_convertor() : model("type convertor")
  {
    add_input<IN>(_name + "_IN", IN{});
    add_output<OUT>(_name + "_OUT", OUT{});
  }

  void update(std::uint64_t) override
  {
    if constexpr (std::is_same<IN, OUT>::value)
    {
      set_output<OUT>(0, std::move(get_input<IN>(0)));
    }
    else
    {
      const IN &in = get_input<IN>(0);
      set_output<OUT>(0, static_cast<OUT>(in));
    }
  }

  bool is_source() const override { return false; }
};

}