#include "model.hpp"

namespace mbd
{
model::model(const std::string& n) : msg_dispatcher(), node(), _name(n) {}

const std::string& model::get_name() const
{
	return _name;
}

bool model::connect(std::uint64_t this_out, const model::ptr_t& other, std::uint64_t other_in)
{
	connection_state state = node::connect(this_out, other.get(), other_in);

	const std::string status = "Connecting '" + _name + "' [out port '" + std::to_string(this_out) + "'] to '" + other->get_name() + "' [in port '" + std::to_string(other_in) + "']";
	log_connection_state(connection_state::CONNECTED, state, status);

	return state == connection_state::CONNECTED;
}

bool model::disconnect(std::uint64_t this_out, const model::ptr_t& other, std::uint64_t other_in)
{
	connection_state state = node::disconnect(this_out, other.get(), other_in);

	const std::string status = "Disconnecting '" + _name + "' [out port '" + std::to_string(this_out) + "'] from '" + other->get_name() + "' [in port '" + std::to_string(other_in) + "']";
	log_connection_state(connection_state::NOT_CONNECTED, state, status);

	return state == connection_state::NOT_CONNECTED;
}

void model::log_connection_state(connection_state expected, connection_state current, const std::string& status)
{
	if (expected == current)
		add_message(log_level::INFO, status + " succeeded. " + connection_info(current));
	else
		add_message(log_level::ERROR, status + " failed. " + connection_info(current));
}

} // namespace mbd
