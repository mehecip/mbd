#include "library.hpp"
#include "model.hpp"

namespace mbd
{

lib::lib(const std::string &name) : _name(name) {}

model::ptr_t lib::build_model(const std::string &name) const
{
  const auto &it = _factories.find(name);
  if (it != _factories.end())
  {
    const auto &factory = it->second;
    return factory();
  }

  return nullptr;
}

const std::vector<std::string> &lib::get_model_names() const
{
  return _model_names;
}

const std::string &lib::get_name() const { return _name; }

} // namespace mbd