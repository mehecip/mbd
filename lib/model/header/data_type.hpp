#pragma once

#include <memory>
#include <typeindex>

namespace mbd
{

struct i_data_type
{
  using ptr_t = std::shared_ptr<i_data_type>;

  virtual ~i_data_type(){};
  virtual const std::type_index get_type() const = 0;
};

template <typename T>
struct data_type_wrapper : public i_data_type
{
  data_type_wrapper(T data) : _data(data){};
  T _data;

  const std::type_index get_type() const override { return typeid(T); }
};

} // namespace mbd