#pragma once

#include "model.hpp"

#include <limits>
#include <type_traits>
#include <typeinfo>

namespace mbd
{
namespace impl
{

template <typename IN, typename OUT>
class type_convertor : public model
{

public:
  type_convertor(const std::string &name) : model(name)
  {
    add_input<IN>(_name + "_IN", IN{});
    add_output<OUT>(_name + "_OUT", OUT{});
  }

  void update(std::uint64_t tick) override
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

  bool is_feedthrough() const override { return true; }
};

} // namespace impl
} // namespace mbd
