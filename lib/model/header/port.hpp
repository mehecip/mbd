#pragma once
#include "data_holder.hpp"
#include "uuid.hpp"
#include <string>

namespace mbd
{

enum class port_dir_t : bool
{
  IN = 0,
  OUT = 1
};

class  port : public data_holder
{
public:

  port() = default;
  port(const std::string &name, port_dir_t d,
       const mbd::uuid &uuid = mbd::uuid());

  port(port &&other) noexcept;
  port &operator=(const port &&other);

  ~port() = default;

  bool operator==(const port &other) const;

  port_dir_t get_dir() const;

  void set_name(const std::string &name);

  void set_connected(bool flag);

  bool is_connected() const;

  const std::string &get_name() const;

  const mbd::uuid &get_uuid() const;

private:
  std::string _name;
  mbd::uuid _uuid;

  port_dir_t _dir;
  bool _connected;
};

} // namespace mbd
