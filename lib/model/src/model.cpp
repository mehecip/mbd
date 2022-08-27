#include "model.hpp"

namespace mbd
{
model::model(const std::string &n) : msg_dispatcher(), node(n) {}

const std::string &model::get_name() const { return _name; }
} // namespace mbd
