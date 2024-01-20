#include "msg_dispatcher.hpp"

#include <algorithm>

namespace mbd
{

void msg_dispatcher::add_message(log_level lvl, const std::string &msg)
{
  std::for_each(_msg_callbacks.begin(), _msg_callbacks.end(),
                [&](const auto &callback) { callback(lvl, msg); });
}

void msg_dispatcher::add_msg_callback(const msg_callback_f &f)
{
  _msg_callbacks.push_back(f);
}

} // namespace mbd
