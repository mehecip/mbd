#include "uuid.hpp"

#include <string>

#include <random>
#include <sstream>

namespace
{
static std::random_device device;
static std::mt19937 generator(device());
static std::uniform_int_distribution<> distribution_0_15(0, 15);
static std::uniform_int_distribution<> distribution_8_11(8, 11);

std::string generate_uuid_v4()
{

  std::stringstream ss;
  std::uint8_t i;

  ss << std::hex;
  for (i = 0; i < 8; i++)
  {
    ss << distribution_0_15(generator);
  }

  ss << "-";
  for (i = 0; i < 4; i++)
  {
    ss << distribution_0_15(generator);
  }

  ss << "-4";
  for (i = 0; i < 3; i++)
  {
    ss << distribution_0_15(generator);
  }

  ss << "-";
  ss << distribution_8_11(generator);
  for (i = 0; i < 3; i++)
  {
    ss << distribution_0_15(generator);
  }

  ss << "-";
  for (i = 0; i < 12; i++)
  {
    ss << distribution_0_15(generator);
  }

  return ss.str();
}
} // namespace

namespace mbd
{

uuid::uuid() : _uuid(generate_uuid_v4()) {}

uuid::uuid(const std::string &uuid) : _uuid(uuid) {}

bool uuid::operator==(const uuid &other) const { return _uuid == other._uuid; }

bool uuid::operator!=(const uuid &other) const { return !operator==(other); }

bool uuid::operator<(const uuid &other) const { return _uuid < other._uuid; }

const std::string &uuid::to_string() const { return _uuid; }
} // namespace mbd