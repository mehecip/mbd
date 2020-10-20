#pragma once
#include <cstdint>
#include <string>

namespace mbd
{
	
enum class connection_state : std::uint8_t
{
	VALID = 0,
	INVALID,
	CONNECTED,
	NOT_CONNECTED,
	ERR_SAME_DIRECTION,
	ERR_DIFF_TYPE,
	ERR_UNKNOWN,
	ERR_IN_PORT_INVALID,
	ERR_OUT_PORT_INVALID,
	ERR_IN_PORT_CONNECTED,

};

const inline std::string connection_info(connection_state state)
{
	const std::unordered_map<connection_state, std::string> info =
	{
		{connection_state::VALID, "valid"},
		{connection_state::INVALID, "invalid"},
		{connection_state::CONNECTED, "connected"},
		{connection_state::NOT_CONNECTED, "not connected"},
		{connection_state::ERR_SAME_DIRECTION, "error same port direction"},
		{connection_state::ERR_DIFF_TYPE, "ports have different types"},
		{connection_state::ERR_UNKNOWN, "unknown error"},
		{connection_state::ERR_IN_PORT_INVALID, "input port index is invalid"},
		{connection_state::ERR_OUT_PORT_INVALID, "output port index is invalid"},
		{connection_state::ERR_IN_PORT_CONNECTED, "input port is already connected"},
	};

	return info.at(state);
}


} // namespace mbd