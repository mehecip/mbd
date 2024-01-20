#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include "param.hpp"
#include "port.hpp"

namespace mbd
{
class uuid; // fwd

struct node
{
  node() = default;
  node(const std::string &name, mbd::uuid uuid = mbd::uuid());

  node(node &&other);
  node &operator=(node &&other);

  ~node() = default;

  // Add a new output of type T
  template <typename T>
  void add_output(const std::string &name, const T &init_data);

  // Add a new input of type T
  template <typename T>
  void add_input(const std::string &name, const T &init_data);

  // Add a new param of type T
  template <typename T>
  void add_param(const std::string &name, const T &val, bool is_readonly = false);

  // Sets the ouput to value data of type T at index
  // Use for best performance
  template <typename T>
  void set_output(std::size_t index, const T &data) const;

  // Sets the ouput with name to value data of type T
  // Least performant
  template <typename T>
  void set_output(const std::string &name, const T &val) const;

  // Reads the  data of type T of the input at index
  // Use for best performance
  template <typename T>
  const T &get_input(std::size_t index) const;

  // Reads the data of type T of the input with name
  // Least performant
  template <typename T>
  const T &get_input(const std::string &name) const;

  // Reads the data of type T of the param at index
  // Use for best performance
  template <typename T>
  const T &get_param(std::size_t index) const;

  // Reads the data of type T of the param with name
  // Least performant
  template <typename T>
  const T &get_param(const std::string &name) const;

  // Sets the data of type T of the param at index
  // Use for best performance
  template <typename T>
  void set_param(std::size_t index, const T &val) const;

  // Sets the data of type T of the param at index
  // Least performant
  template <typename T>
  void set_param(const std::string &name, const T &val) const;

  std::string _name;
  mbd::uuid _uuid;

  std::vector<port> _in_ports, _out_ports;
  std::vector<param> _params;

  using name_to_idx_t = std::unordered_map<std::string, std::size_t>;
  name_to_idx_t _in_ports_map, _out_ports_map, _params_map;

  bool operator<(const node &other) const;
  bool operator==(const node &other) const;
};

/**************************************************PORTS********************************************************/

template <typename T>
inline void node::add_input(const std::string &name, const T &init_data)
{
  port p(name, port_dir_t::IN);
  p.set_data<T>(init_data);

  _in_ports.push_back(std::move(p));
  _in_ports_map.emplace(name, _in_ports.size() - 1ul);
}

template <typename T>
inline void node::add_output(const std::string &name, const T &init_data)
{
  port p(name, port_dir_t::OUT);
  p.set_data<T>(init_data);

  _out_ports.push_back(std::move(p));
  _out_ports_map.emplace(name, _out_ports.size() - 1ul);
}

template <typename T>
inline void node::set_output(std::size_t index, const T &data) const
{
  _out_ports[index].write_data<T>(data);
}

template <typename T>
inline const T &node::get_input(std::size_t index) const
{
  return _in_ports[index].read_data<T>();
}

template <typename T>
inline const T &node::get_input(const std::string &name) const
{
  const auto &index = _in_ports_map.at(name);
  return _in_ports[index].read_data<T>();
}

template <typename T>
inline void node::set_output(const std::string &name, const T &val) const
{
  const auto &index = _out_ports_map.at(name);
  _out_ports[index].write_data<T>(val);
}

/**************************************************PARAMS********************************************************/

template <typename T>
inline void node::add_param(const std::string &name, const T &val, bool is_readonly)
{
  param p(name, is_readonly);
  p.set_data<T>(val);

  _params.push_back(std::move(p));
  _params_map.emplace(name, _params.size() - 1ul);
}

template <typename T>
inline const T &node::get_param(const std::string &name) const
{
  const auto &index = _params_map.at(name);
  return _params[index].read_data<T>();
}

template <typename T>
inline void node::set_param(const std::string &name, const T &val) const
{
  const auto &index = _params_map.at(name);
  _params[index].write_data<T>(val);
}

template <typename T>
inline const T &node::get_param(std::size_t index) const
{
  return _params[index].read_data<T>();
}

template <typename T>
inline void node::set_param(std::size_t index, const T &val) const
{
  _params[index].write_data<T>(val);
}

} // namespace mbd
