#pragma once
#include <cstdint>
#include <string>

namespace mbd
{
	
enum class log_level : std::uint8_t
{
	INFO = 0,
	WARNING = 1,
	ERROR = 2
};

const inline std::string level_info(log_level lvl)
{
	const std::unordered_map<log_level, std::string> info =
	{
		{log_level::INFO, "[INFO]"},
		{log_level::WARNING, "[WARN]"},
		{log_level::ERROR, "[ERROR]"}
	};

	return info.at(lvl);
}

} // namespace mbd