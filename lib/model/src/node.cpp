#include "node.hpp"
#include "connection.hpp"
#include "connection_state.hpp"

namespace mbd
{

node::node(const std::string &name, mbd::uuid uuid)
    : _name(name), _uuid(uuid){};

bool node::operator<(const node &other) const { return _uuid < other._uuid; }

node::node(node &&other)
{
  std::swap(_in_ports, other._in_ports);
  std::swap(_out_ports, other._out_ports);
  std::swap(_params, other._params);
  _name = other._name;
  _uuid = other._uuid;
}

node &node::operator=(node &&other)
{
  std::swap(_in_ports, other._in_ports);
  std::swap(_out_ports, other._out_ports);
  std::swap(_params, other._params);
  _name = other._name;
  _uuid = other._uuid;

  return *this;
}

bool node::operator==(const node &other) const
{
  // The uuid should be enough here, but let's be that guy
  return _uuid == other._uuid && _in_ports == other._in_ports &&
         _out_ports == other._out_ports && _params == other._params;
}
} // namespace mbd
