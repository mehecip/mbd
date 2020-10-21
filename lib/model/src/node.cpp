#pragma once
#include "node.hpp"

namespace mbd
{

connection_state node::connect(std::uint64_t this_out, const std::unique_ptr<node>& other, std::uint64_t other_in)
{
	connection_state state = validate(this_out, other, other_in);
	if (state != connection_state::VALID)
		return state;

	const auto this_port = _out_ports[this_out];
	const auto other_port = other->_in_ports[other_in];

	if (other_port->is_connected())
		return connection_state::ERR_IN_PORT_CONNECTED;

	this_port->set_connected(true);
	other_port->set_connected(true);

	_connections[this_port].push_back(other_port);
	return connection_state::CONNECTED;
}

connection_state node::disconnect(std::uint64_t this_out, const std::unique_ptr<node>& other, std::uint64_t other_in)
{
	connection_state state = validate(this_out, other, other_in);
	if (state != connection_state::VALID)
		return state;

	const auto this_port = _out_ports[this_out];
	const auto other_port = other->_in_ports[other_in];

	auto& it = _connections.find(this_port);
	if (it != _connections.end())
	{
		auto& inputs = (*it).second;
		inputs.erase(std::remove(inputs.begin(), inputs.end(), other_port), inputs.end());

		if (inputs.size() == 0)
		{
			this_port->set_connected(false);
			_connections.erase(it);
		}

		other_port->set_connected(false);
		return connection_state::NOT_CONNECTED;
	}

	return connection_state::ERR_PORT_NOT_CONNECTED;
}

connection_state node::validate(std::uint64_t this_out, const std::unique_ptr<node>& other, std::uint64_t other_in)
{
	if (this_out >= _out_ports.size())
		return connection_state::ERR_OUT_PORT_INVALID;

	if (other_in >= other->_in_ports.size())
		return connection_state::ERR_IN_PORT_INVALID;

	const auto this_port = _out_ports[this_out];
	const auto other_port = other->_in_ports[other_in];

	if (this_port->get_dir() == other_port->get_dir())
		return connection_state::ERR_SAME_DIRECTION;

	if (this_port->get_data_type() != other_port->get_data_type())
		return connection_state::ERR_DIFF_TYPE;

	return connection_state::VALID;
}

} // namespace mbd
