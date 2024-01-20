#pragma once

#include <memory>
#include <string>
#include <typeindex>
#include <utility>

#include "data_type.hpp"

namespace mbd
{
// A type that holds some i_data_type data and provides methods to access it
class data_holder
{
public:
  data_holder() = default;
  data_holder(data_holder &&other) noexcept;

  ~data_holder() = default;

  const std::type_index get_data_type() const;

  template <typename T>
  void set_data(const T &data);

  template <typename T>
  void write_data(const T &data) const;

  template <typename T>
  const T &read_data() const;

private:
  // mbd::connection changes the input port data pointer to the output port data
  // pointer so that the connection is made
  friend class connection;

  i_data_type::ptr_t _data;
};

template <typename T>
inline void data_holder::set_data(const T &data)
{
  _data = std::make_unique<data_type_wrapper<T>>(data);
}

template <typename T>
inline void data_holder::write_data(const T &data) const
{
  static_cast<data_type_wrapper<T> *>(_data.get())->_data = data;
}

template <typename T>
inline const T &data_holder::read_data() const
{
  return static_cast<data_type_wrapper<T> *>(_data.get())->_data;
}

} // namespace mbd
