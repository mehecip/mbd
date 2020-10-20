#pragma once
#include <cstdint>
#include <string>

#include <vector>
#include <memory>

#include <unordered_map>

#include <algorithm>

#include "component.hpp"

namespace mbd
{
component::component(const std::string& n) : _name(n), _node(std::make_unique<node>()) {}

const std::string& component::get_name() const
{
	return _name;
}

bool component::connect(std::uint64_t this_out, const std::unique_ptr<component>& other, std::uint64_t other_in)
{
	connection_state state = _node->connect(this_out, other->_node, other_in);

	const std::string status = "'" + _name + "' [out port '" + std::to_string(this_out) + "'] to '" + other->get_name() + "' [in port '" + std::to_string(other_in) + "'] status:";
	log_connection_state(connection_state::CONNECTED, state, status);

	return state == connection_state::CONNECTED;
}

bool component::disconnect(std::uint64_t this_out, const std::unique_ptr<component>& other, std::uint64_t other_in)
{
	connection_state state = _node->disconnect(this_out, other->_node, other_in);

	const std::string status = "'" + _name + "' [out port '" + std::to_string(this_out) + "'] to '" + other->get_name() + "' [in port '" + std::to_string(other_in) + "'] status: ";
	log_connection_state(connection_state::NOT_CONNECTED, state, status);

	return state == connection_state::NOT_CONNECTED;
}

void component::add_message(log_level lvl, const std::string& msg)
{
	std::for_each(_msg_callbacks.begin(), _msg_callbacks.end(), [&](const auto& callback) {callback(lvl, msg); });
}

void component::add_msg_callback(const msg_callbacks_t& f)
{
	_msg_callbacks.push_back(f);
}

void component::log_connection_state(connection_state expected, connection_state current, const std::string& status)
{
	log_level lvl = (expected == current ? log_level::INFO : log_level::WARNING);
	add_message(lvl, status + " " + connection_info(current));
}

} // namespace mbd
