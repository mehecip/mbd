#include "port.hpp"
#include "data_holder.hpp"
#include "uuid.hpp"

namespace mbd
{
port::port(const std::string &name, port_dir_t d, const mbd::uuid &uuid)
    : data_holder(), _name(name), _uuid(uuid), _dir(d), _connected(false)
{}

port::port(port &&other) noexcept
    : data_holder(std::move(other)), _name(std::move(other._name)),
      _uuid(other._uuid), _dir(other._dir), _connected(other._connected)
{}

port &port::operator=(const port &&other)
{
  _dir = other._dir;
  _name = other._name;
  _uuid = other._uuid;
  _connected = other._connected;
  return *this;
};

port_dir_t port::get_dir() const { return _dir; }

void port::set_name(const std::string &name) { _name = name; }

const std::string &port::get_name() const { return _name; }

const mbd::uuid &port::get_uuid() const { return _uuid; }

void port::set_connected(bool flag) { _connected = flag; }

bool port::is_connected() const { return _connected; }

bool port::operator==(const port &other) const
{
  return _name == other._name && _dir == other._dir && _uuid == other._uuid &&
         _connected == other._connected;
}

} // namespace mbd
