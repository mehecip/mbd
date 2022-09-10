#include "library.hpp"
#include "model.hpp"
#include <unordered_set>

namespace mbd
{

lib::lib(const std::string &name) : _lib_name(name) {}

model::ptr_t lib::build_model(const std::string &name) const
{
  const auto it = _factories.find(name);
  if (it != _factories.end())
  {
    const auto &factory = it->second;
    return factory();
  }

  return nullptr;
}

const std::unordered_set<std::string> &lib::get_model_names() const
{
  return _model_names;
}

const std::string &lib::get_name() const { return _lib_name; }

} // namespace mbd