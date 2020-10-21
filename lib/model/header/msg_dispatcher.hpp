#pragma once
#include <functional>
#include <string>
#include <algorithm>

#include "node.hpp"
#include "log_level.hpp"

namespace mbd
{
using msg_callbacks_t = std::function<void(log_level, const std::string&)>;

// I did not know how to call it :|
class msg_dispatcher
{

public:
	msg_dispatcher() = default;

	~msg_dispatcher() {};

	void add_msg_callback(const msg_callbacks_t& f);

protected:
	void add_message(log_level lvl, const std::string& msg);

private:
	std::vector<msg_callbacks_t> _msg_callbacks;
};
} // namespace mbd
