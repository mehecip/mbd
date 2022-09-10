#include "node.hpp"
#include "connection.hpp"
#include "connection_state.hpp"

namespace mbd
{

node::node(const std::string &name, mbd::uuid uuid)
    : _name(name), _uuid(uuid){};

bool node::operator<(const node &other) const { return _uuid < other._uuid; }

bool node::operator==(const node &other) const { return _uuid == other._uuid; }
} // namespace mbd
