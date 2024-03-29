#include "model.hpp"

namespace mbd
{
model::model(const std::string &n) : msg_dispatcher(), node(n) {}

const std::string &model::get_name() const { return node::_name; }

void model::set_name(const std::string &name) { node::_name = name; }

} // namespace mbd
