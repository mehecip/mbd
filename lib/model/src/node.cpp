#include "node.hpp"

namespace mbd
{

connection_state node::connect(std::uint64_t this_out, const std::unique_ptr<node>& other, std::uint64_t other_in)
{
	connection_state state = validate(this_out, other, other_in);
	if (state != connection_state::VALID)
		return state;

	const auto other_port = other->_in_ports[other_in].get();

	if (other_port->is_connected())
		return connection_state::ERR_IN_PORT_CONNECTED;

	other_port->set_connected(true);

	_output_connections[this_out].push_back(other_port);
	return connection_state::CONNECTED;
}

connection_state node::disconnect(std::uint64_t this_out, const std::unique_ptr<node>& other, std::uint64_t other_in)
{
	connection_state state = validate(this_out, other, other_in);
	if (state != connection_state::VALID)
		return state;

	if(_output_connections[this_out].size() == 0)
		return connection_state::ERR_PORT_NOT_CONNECTED;

	const auto other_port = other->_in_ports[other_in].get();

	auto& inputs = _output_connections[this_out];
	inputs.erase(std::remove(inputs.begin(), inputs.end(), other_port), inputs.end());

	other_port->set_connected(false);
	return connection_state::NOT_CONNECTED;
}

connection_state node::validate(std::uint64_t this_out, const std::unique_ptr<node>& other, std::uint64_t other_in)
{
	if (this_out >= _out_ports.size())
		return connection_state::ERR_OUT_PORT_INVALID;

	if (other_in >= other->_in_ports.size())
		return connection_state::ERR_IN_PORT_INVALID;

	const auto this_port = _out_ports[this_out].get();
	const auto other_port = other->_in_ports[other_in].get();

	if (this_port->get_dir() == other_port->get_dir())
		return connection_state::ERR_SAME_DIRECTION;

	if (this_port->get_data_type() != other_port->get_data_type())
		return connection_state::ERR_DIFF_TYPE;

	return connection_state::VALID;
}

} // namespace mbd
