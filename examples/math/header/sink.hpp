#pragma once
#include "model.hpp"

namespace mbd
{
namespace impl
{

template <typename T>
class sink : public model
{
public:
  sink() : model("sink")
  {
    model::add_input<T>(model::_name + "_In", T{});
    model::add_param(_value, T{});
  }

  void update(std::uint64_t tick) override
  {
    const auto val = model::get_input<T>(0);
    model::set_param<T>(_value, val);
  }

  bool is_source() const override { return false; }

  T read() const { return model::get_param<T>(_value); }

private:
  const std::string _value = "val";
};

} // namespace impl
} // namespace mbd