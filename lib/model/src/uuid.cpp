#include "uuid.hpp"

#include <string>

namespace mbd
{
uuid::uuid(const std::string &str) : _uuid(str){};

bool uuid::operator==(uuid &other) const { return _uuid == other._uuid; }

const std::string &uuid::to_string() const { return _uuid; }
} // namespace mbd