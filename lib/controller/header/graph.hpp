#pragma once

#include <algorithm>
#include <cstdint>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>


#include "connection.hpp"
#include "dfs.hpp"
#include "log_level.hpp"
#include "model.hpp"

namespace mbd
{

class  graph
{
public:
  graph() = default;

  void push_model(model::ptr_t &&m);

  bool connect(const std::string &out_model, std::uint64_t out_idx,
               const std::string &in_model, std::uint64_t in_idx);

  bool disconnect(const std::string &out_model, std::uint64_t out_idx,
                  const std::string &in_model, std::uint64_t in_idx);

  template <typename T>
  inline T *get(const std::string &name);

  template <typename ALGO = dfs>
  std::vector<model_vec_t> algebraic_loops();

  template <typename ALGO = dfs>
  std::vector<model_vec_t> execution_order();

  const std::vector<model::ptr_t>& get_models() const {
    return _models;
  }

  const std::vector<connection::ptr_t>& get_connections() const {
    return _connections;
  }

private:
  // if it exists, finds a model in the _model_cache by name
  // otherwise searches in the _models vector and updates the cache
  model *find_model(const std::string &name);

  std::vector<model::ptr_t> _models;
  std::vector<connection::ptr_t> _connections;
  std::vector<model_vec_t> _ex_order_cache, _algeb_loops_cache;
  std::unordered_map<std::string, model *> _models_cache;

  bool _dirty_algeb_cache = true, _dirty_order_cache = true;
};

template <typename T>
inline T *graph::get(const std::string &name)
{
  model *mdl = find_model(name);
  if (mdl != nullptr)
  {
    return dynamic_cast<T *>(mdl);
  }

  return nullptr;
}

template <typename ALGO>
inline std::vector<model_vec_t> graph::algebraic_loops()
{
  if (_dirty_algeb_cache)
  {
    ALGO alg(_models, _connections);
    _algeb_loops_cache = alg.algebraic_loops();
    _dirty_algeb_cache = false;
  }

  return _algeb_loops_cache;
}

template <typename ALGO>
inline std::vector<model_vec_t> graph::execution_order()
{
  if (_dirty_order_cache)
  {
    ALGO alg(_models, _connections);

    if (_dirty_algeb_cache)
    {
      _algeb_loops_cache = alg.algebraic_loops();
      _dirty_algeb_cache = false;
    }

    _dirty_order_cache = false;
    _ex_order_cache = _algeb_loops_cache.size() == 0ul
                          ? alg.order()
                          : std::vector<model_vec_t>{};
  }
  return _ex_order_cache;
}

} // namespace mbd