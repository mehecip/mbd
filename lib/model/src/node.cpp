#include "node.hpp"
#include "connection.hpp"
#include "connection_state.hpp"
#include <cstdint>

namespace
{
std::uint64_t get_index(const std::vector<mbd::port>& v, const mbd::port* const p)
{
  const auto &it = std::find(v.cbegin(), v.cend(), *p);
  return it - v.cbegin();
}
} // namespace

namespace mbd
{

node::node(const std::string &name, mbd::uuid uuid)
    : _name(name), _uuid(uuid){};

void node::connect(port *this_out, port *other_in)
{
  const auto idx = get_index(_out_ports, other_in);
  _output_connections[idx].push_back(other_in);
}

void node::disconnect(port *this_out, port *other_in)
{
  const auto idx = get_index(_out_ports, other_in);

  auto &ports = _output_connections[idx];
  const auto o_it = std::find(ports.cbegin(), ports.cend(), other_in);
  if (o_it != ports.end())
    ports.erase(o_it);
}
} // namespace mbd
