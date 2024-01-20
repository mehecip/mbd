#pragma once

#include "model.hpp"

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace mbd
{

class  lib
{
public:
  lib(const std::string &name);

  /* Registers a model builder with the same type as the model.*/
  template <typename M, typename... Args>
  constexpr inline void register_model(const std::string &type, Args &&...args);

  /* Calls a model builder with the same type as the model.*/
  model::ptr_t build_model(const std::string &type) const;

  /* Returns a list of all the models that can be built.*/
  const std::unordered_set<std::string> &get_model_types() const;

  /* Returns the name of the lib.*/
  const std::string &get_name() const;

private:
  using factory_t = std::function<model::ptr_t()>;

  std::unordered_map<std::string, factory_t> _factories;
  std::unordered_set<std::string> _model_types;
  std::string _lib_name;
};

template <typename M, typename... Args>
constexpr inline void lib::register_model(const std::string &type,
                                          Args &&...args)
{
  if (_factories.find(type) != _factories.end())
    return;

  const auto &factory = [=]() {
    return std::make_unique<M>(std::move(args)...);
  };

  _factories[type] = std::move(factory);
  _model_types.insert(type);
}
} // namespace mbd