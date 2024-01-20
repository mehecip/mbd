#pragma once
#include <string>

#include "data_holder.hpp"
#include "uuid.hpp"

namespace mbd
{

class param : public data_holder
{
public:
  param() = default;
  param(const std::string &name, bool is_readonly = false, mbd::uuid uuid = mbd::uuid());

  param(param &&other) noexcept;
  param &operator=(const param &&other);

  ~param() = default;

  bool operator==(const param &other) const;

  void set_name(const std::string &name);
  const std::string &get_name() const;
  const mbd::uuid &get_uuid() const;
  bool is_readonly() const;

private:
  std::string _name;
  bool _is_readonly;
  mbd::uuid _uuid;
};

} // namespace mbd
