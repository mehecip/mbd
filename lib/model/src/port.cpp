#include "port.hpp"
#include "data_holder.hpp"

namespace mbd
{
port::port(const std::string &name, port_dir_t d)
    : data_holder(), _dir(d), _connected(false), _name(name)
{}

port::port(port &&other) noexcept
    : data_holder(std::move(other)), _name(std::move(other._name)),
      _dir(other._dir), _connected(other._connected)
{}

const port_dir_t port::get_dir() const { return _dir; }

void port::set_name(const std::string &name) { _name = name; }

const std::string &port::get_name() const { return _name; }

void port::set_connected(bool flag) { _connected = flag; }

bool port::is_connected() const { return _connected; }

bool port::operator==(const port &other) const
{
  return _name == other._name && _dir == other._dir;
}

} // namespace mbd
