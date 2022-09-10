#pragma once

#include "model.hpp"

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace mbd
{

class lib
{
public:
  lib(const std::string &name);

  /* Registers a model builder with the same name as the model.*/
  template <typename M, typename... Args>
  constexpr inline void register_model(const std::string &name, Args &&...args);

  /* Calls a model builder with the same name as the model.*/
  model::ptr_t build_model(const std::string &name) const;

  /* Returns a list of all the models that can be built.*/
  const std::unordered_set<std::string> &get_model_names() const;

  /* Returns the name of the lib.*/
  const std::string &get_name() const;

private:
  using factory_t = std::function<model::ptr_t()>;

  std::unordered_map<std::string, factory_t> _factories;
  std::unordered_set<std::string> _model_names;
  std::string _lib_name;
};

template <typename M, typename... Args>
constexpr inline void lib::register_model(const std::string &name,
                                          Args &&...args)
{
  if (_factories.find(name) != _factories.end())
    return;

  const auto &factory = [=]() {
    return std::make_unique<M>(name, std::move(args)...);
  };

  _factories[name] = std::move(factory);
  _model_names.insert(name);
}
} // namespace mbd