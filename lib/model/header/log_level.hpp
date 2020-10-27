#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>

#include <enum_hash.hpp>

namespace mbd
{
	
enum class log_level : std::uint8_t
{
	DEBUG = 0,
	INFO,
	WARNING,
	ERROR
};

const inline std::string level_info(log_level lvl)
{
	const std::unordered_map<log_level, std::string, enum_class_hash> info =
	{
		{log_level::DEBUG, "[DEBUG]"},
		{log_level::INFO, "[INFO]"},
		{log_level::WARNING, "[WARN]"},
		{log_level::ERROR, "[ERROR]"}
	};

	return info.at(lvl);
}

} // namespace mbd