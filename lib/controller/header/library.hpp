#pragma once

#include "model.hpp"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace mbd
{

class lib
{
public:
  lib(const std::string &name);

  /* Registers a model builder with the same name as the model.*/
  template <typename M, typename... Args>
  inline void register_model(const std::string &name, Args &&...args);

  /* Calls a model builder with the same name as the model.*/
  model::ptr_t build_model(const std::string &name) const;

  /* Returns a list of all the models that can be built.*/
  const std::vector<std::string> &get_model_names() const;

  /* Returns the name of the lib.*/
  const std::string &get_name() const;

private:
  using factory_t = std::function<model::ptr_t()>;

  std::unordered_map<std::string, factory_t> _factories;
  std::vector<std::string> _model_names;
  std::string _name;
};

template <typename M, typename... Args>
inline void lib::register_model(const std::string &name, Args &&...args)
{
  if (_factories.find(name) != _factories.end())
    return;

  const factory_t &factory = [&]() {
    return std::make_unique<M>(name, std::forward<Args>(args)...);
  };

  _factories[name] = std::move(factory);
  _model_names.push_back(name);
}
} // namespace mbd