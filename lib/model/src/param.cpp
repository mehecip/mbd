#include "param.hpp"
#include "data_holder.hpp"

namespace mbd {
param::param(const std::string &name, bool is_readonly, mbd::uuid uuid)
    : data_holder(), _name(name), _is_readonly(is_readonly), _uuid(uuid) {}

param::param(param &&other) noexcept
    : data_holder(std::move(other)), _name(std::move(other._name)),
     _is_readonly(std::move(other._is_readonly)), _uuid(std::move(other._uuid)) {}

param &param::operator=(const param &&other) {
  _name = other._name;
  _is_readonly = other._is_readonly;
  _uuid = other._uuid;
  return *this;
};

const std::string &param::get_name() const { return _name; }

const mbd::uuid &param::get_uuid() const { return _uuid; }

bool param::is_readonly() const { return _is_readonly; }

void param::set_name(const std::string &name) { _name = name; }

bool param::operator==(const param &other) const {
  return _name == other._name && _uuid == other._uuid;
}

} // namespace mbd