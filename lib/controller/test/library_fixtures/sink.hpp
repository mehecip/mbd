#pragma once
#include "model.hpp"

using namespace mbd;

namespace tst
{

template <typename T>
class sink : public model
{
public:
  sink() : model("sink"), _store_data(true)
  {
    model::add_input<T>(model::_name + "_In", T{});
    model::add_param(_values, std::vector<T>{});
  }

  void update(std::uint64_t) override
  {
    if (_store_data)
    {
      const auto val = model::get_input<T>(0);
      auto new_vals = model::get_param<std::vector<T>>(_values);
      new_vals.push_back(val);
      model::set_param<std::vector<T>>(_values, new_vals);
    }
  }

  void store_data(bool flag) { _store_data = flag; }

  bool is_source() const override { return false; }

  std::vector<T> read() const { return model::get_param<std::vector<T>>(_values); }

  void reset()
  {
    model::set_param<T>(_values, std::vector<T>{});
  }

private:
  const std::string _values = "vals";
  bool _store_data = true;
};

} 