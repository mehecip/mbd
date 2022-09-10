#pragma once
#include <algorithm>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include "connection.hpp"
#include "connection_state.hpp"
#include "param.hpp"
#include "port.hpp"
#include "uuid.hpp"

namespace mbd
{

struct node
{
  node(const std::string &name, mbd::uuid uuid = mbd::uuid());
  ~node() = default;

  const mbd::uuid _uuid;
  std::string _name;

  std::vector<port> _in_ports, _out_ports;
  std::unordered_map<std::string, param> _params;

  bool operator<(const node &other) const;
  bool operator==(const node &other) const;

protected:
  template <typename T>
  void set_output(std::uint64_t index, const T &data) const;

  template <typename T>
  const T &get_input(std::uint64_t index) const;

  template <typename T>
  void add_input(const std::string &name, const T &init_data);

  template <typename T>
  void add_output(const std::string &name, const T &init_data);

  template <typename T>
  void add_param(const std::string &name, const T &val);

  template <typename T>
  const T &get_param(const std::string &name) const;

  template <typename T>
  void set_param(const std::string &name, const T &val);

private:
  std::vector<std::vector<port *>> _output_connections;
};

/**************************************************PORTS********************************************************/

template <typename T>
inline void node::set_output(std::uint64_t index, const T &data) const
{
  _out_ports[index].write_data<T>(data);
}

template <typename T>
inline const T &node::get_input(std::uint64_t index) const
{
  return _in_ports[index].read_data<T>();
}

template <typename T>
inline void node::add_input(const std::string &name, const T &init_data)
{
  port p(name, port_dir_t::IN);
  p.set_data<T>(init_data);

  _in_ports.push_back(std::move(p));
}

template <typename T>
inline void node::add_output(const std::string &name, const T &init_data)
{
  port p(name, port_dir_t::OUT);
  p.set_data<T>(init_data);

  _out_ports.push_back(std::move(p));
  _output_connections.push_back({});
}

/**************************************************PARAMS********************************************************/

template <typename T>
inline void node::add_param(const std::string &name, const T &val)
{
  param p(name);
  p.set_data<T>(val);

  _params.emplace(name, std::move(p));
}

template <typename T>
inline const T &node::get_param(const std::string &name) const
{
  const auto &it = _params.find(name);
  return it->second.read_data<T>();
}

template <typename T>
inline void node::set_param(const std::string &name, const T &val)
{
  const auto &it = _params.find(name);
  it->second.write_data<T>(val);
}

} // namespace mbd
