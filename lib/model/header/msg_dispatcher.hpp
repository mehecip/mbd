#pragma once
#include <functional>
#include <string>
#include <algorithm>
#include <vector>

#include "log_level.hpp"

namespace mbd
{
using msg_callback_f = std::function<void(log_level, const std::string&)>;

// I did not know how to call it :|
class msg_dispatcher
{

public:
	msg_dispatcher() = default;

	void add_msg_callback(const msg_callback_f& f);

protected:
	void add_message(log_level lvl, const std::string& msg);

private:
	std::vector<msg_callback_f> _msg_callbacks;
};
} // namespace mbd
