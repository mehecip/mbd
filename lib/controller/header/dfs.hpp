#pragma once

#include "connection.hpp"
#include "model.hpp"

#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace mbd
{
using model_vec_t = std::vector<model *>;

class dfs
{
  using conn_set_t = std::unordered_set<model *>;
  using conn_map_t = std::unordered_map<model *, conn_set_t>;

  using prio_map_t = std::unordered_map<model *, std::uint64_t>;

public:
  dfs(const std::vector<model::ptr_t> &models,
      const std::vector<connection::ptr_t> &conn);

  std::vector<model_vec_t> order();

  std::vector<model_vec_t> algebraic_loops();

private:
  std::vector<model_vec_t> get_all_loops();
  
  conn_map_t _children, _parents;
  model_vec_t _models;
};

} // namespace mbd