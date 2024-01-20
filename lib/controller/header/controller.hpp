#pragma once


#include <string>
#include <unordered_map>
#include <vector>

#include "graph.hpp"
#include "library.hpp"
#include "msg_dispatcher.hpp"

namespace mbd
{

class model;

class  controller
{
public:
  controller(const msg_callback_f &f);
  ~controller() = default;

  controller(controller &&) = delete;
  controller(const controller &) = delete;

  controller &operator=(controller &&) = delete;
  controller &operator=(const controller &) = delete;

  void add_library(const lib &l);

  model* add_model(const std::string &libname, const std::string &modeltype);

  // connects the out_idx of out_model to the in_idx of in_model
  bool connect(const std::string &out_model, std::uint64_t out_idx,
               const std::string &in_model, std::uint64_t in_idx);

  bool disconnect(const std::string &out_model, std::uint64_t out_idx,
                  const std::string &in_model, std::uint64_t in_idx);

  // an algebraic loop is a loop for which the execution order cannot be
  // computed
  std::size_t find_algebraic_loops();

  void execution_order();

  // synchonous execution(in the order from execution order)
  void run(std::uint64_t ticks);

  // asynchronous execution(in the order from execution order)
  // all the models with the same prio order would be executed at the same time
  // with std::async
  void run_async(std::uint64_t ticks);

  // get a T ptr to the model with key = name
  // or nullptr if T and model type dont match
  // or nullptr if model with key = name is not found in the map
  template <typename T>
  T *get(const std::string &name);

  const mbd::graph& get_graph() const {return _g;}

private:
  msg_callback_f _callback_f;

  graph _g;
  std::unordered_map<std::string, lib> _libs;

  void log_prio(const std::vector<std::vector<model *>> &v);
};

template <typename T>
inline T *controller::get(const std::string &name)
{
  return _g.get<T>(name);
}

} // namespace mbd