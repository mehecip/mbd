#pragma once
#include "model.hpp"

using namespace mbd;

namespace tst
{

template <typename T>
class sink : public model
{
public:
  sink() : model("sink")
  {
    model::add_input<T>(model::_name + "_In", T{});
    model::add_param(_values, std::vector<T>{});
  }

  void update(std::uint64_t) override
  {
    const auto val = model::get_input<T>(0);
    auto new_vals = model::get_param<std::vector<T>>(_values); 
    new_vals.push_back(val);
    model::set_param<std::vector<T>>(_values, new_vals);
  }

  bool is_source() const override { return false; }

  std::vector<T> read() const { return model::get_param<std::vector<T>>(_values); }

  void reset()
  {
    model::set_param<T>(_values, std::vector<T>{});
  }

private:
  const std::string _values = "vals";
};

} 