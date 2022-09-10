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
  sink(const std::string &name) : model(name)
  {
    model::add_input<T>(model::_name + "_In", T{});
    model::add_param(_value, T{});
  }

  void update(std::uint64_t tick) override
  {
    const auto val = model::get_input<T>(0);
    model::set_param<T>(_value, val);
  }

  bool is_feedthrough() const override { return true; }

  T read() const { return model::get_param<T>(_value); }

private:
  const std::string _value = "val";
};

} // namespace impl
} // namespace mbd