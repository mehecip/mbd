#pragma once
#include "component.hpp"

namespace mbd
{
component::component(const std::string& n) : msg_dispatcher(), _name(n), _node(std::make_unique<node>()) {}

const std::string& component::get_name() const
{
	return _name;
}

bool component::connect(std::uint64_t this_out, const component_ptr_t& other, std::uint64_t other_in)
{
	connection_state state = _node->connect(this_out, other->_node, other_in);

	const std::string status = "Connect '" + _name + "' [out port '" + std::to_string(this_out) + "'] to '" + other->get_name() + "' [in port '" + std::to_string(other_in) + "']";
	log_connection_state(connection_state::CONNECTED, state, status);

	return state == connection_state::CONNECTED;
}

bool component::disconnect(std::uint64_t this_out, const component_ptr_t& other, std::uint64_t other_in)
{
	connection_state state = _node->disconnect(this_out, other->_node, other_in);

	const std::string status = "Disconnect '" + _name + "' [out port '" + std::to_string(this_out) + "'] to '" + other->get_name() + "' [in port '" + std::to_string(other_in) + "']";
	log_connection_state(connection_state::NOT_CONNECTED, state, status);

	return state == connection_state::NOT_CONNECTED;
}

void component::log_connection_state(connection_state expected, connection_state current, const std::string& status)
{
	if (expected == current)
		add_message(log_level::INFO, status + " succeeded. " + connection_info(current));
	else
		add_message(log_level::WARNING, status + " failed. " + connection_info(current));
}

} // namespace mbd
