#pragma once

#include <string>

namespace mbd
{
class uuid
{
public:
  /* Uses a built-in method that is 'good enough' to generate uuids*/
  uuid();

  /* Pass your uuid as string */
  uuid(const std::string &str);

  bool operator==(const uuid &other) const;
  bool operator!=(const uuid &other) const;
  bool operator<(const uuid &other) const;

  const std::string &to_string() const;

private:
  std::string _uuid;
};
} // namespace mbd