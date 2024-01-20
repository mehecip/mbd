#include "dfs.hpp"

#include <algorithm>
#include <cstdint>
#include <stack>
#include <vector>

namespace mbd {

dfs::dfs(const std::vector<model::ptr_t> &models,
         const std::vector<connection::ptr_t> &conn) {

  _models.reserve(models.size());

  for (const auto &m : models)
    _models.push_back(m.get());

  for (const auto &c : conn) {
    const auto [from, to] = c->get_models();
    _children[from].insert(to);
    _parents[to].insert(from);
  }
}

std::vector<model_vec_t> dfs::order() {
  model_vec_t sources;
  prio_map_t prio_map;

  // add all the models that are not feedthrough with prio 0
  // they will act as starting points in the bfs algo
  std::for_each(_models.cbegin(), _models.cend(), [&](const auto &m) {
    if (m->is_source()) {
      sources.push_back(m);
    }
  });

  const auto &get_model_prio =
      [&](auto *m, std::uint64_t current_prio) -> std::uint64_t {
    if (!m->is_source()) // is source
    {
      return 0ul;
    }

    if (const auto it = prio_map.find(m); it != prio_map.end()) {
      std::size_t m_prio = it->second;
      return std::max(m_prio, current_prio) + 1ul;
    }

    return ++current_prio;
  };

  for (const auto start : sources) {
    std::stack<model *> stack;
    stack.push(start);

    std::size_t prio = 0;
    std::unordered_set<model *> visited;

    // bfs
    while (!stack.empty()) {
      auto model = stack.top();
      stack.pop();

      visited.insert(model);

      prio = get_model_prio(model, prio);
      prio_map[model] = prio;

      for (auto next : _children[model]) {
        if (visited.find(next) == visited.end())
          stack.push(next);
      }
    }
  }

  // at most _models.size() elements
  std::vector<model_vec_t> priority_vect;
  priority_vect.resize(_models.size());

  for (const auto &m : prio_map)
    priority_vect[m.second].push_back(m.first);

  priority_vect.erase(
      std::remove_if(priority_vect.begin(), priority_vect.end(),
                     [](const auto &el) { return el.size() == 0; }),
      priority_vect.end());

  return priority_vect;
}

std::vector<model_vec_t> dfs::algebraic_loops() {
  std::vector<model_vec_t> loops = get_all_loops();

  // none of the models in a loop are sources
  // then the loop is algebraic and we keep it
  loops.erase(std::remove_if(loops.begin(), loops.end(),
                             [](const auto &l) {
                               return !std::all_of(l.begin(), l.end(),
                                                   [](const auto &m) {
                                                     return !m->is_source();
                                                   });
                             }),
              loops.end());

  std::sort(loops.begin(), loops.end(), [](auto &lhs, auto &rhs) {
    std::sort(lhs.begin(), lhs.end());
    std::sort(rhs.begin(), rhs.end());

    return lhs < rhs;
  });

  loops.erase(std::unique(loops.begin(), loops.end()), loops.end());
  return loops;
}

std::vector<model_vec_t> dfs::get_all_loops() {
  std::vector<model_vec_t> loops;

  for (const auto start : _models) {
    // a model can't be in a loop if it has no inputs or outputs connected
    if (_children.find(start) == _children.end() ||
        _parents.find(start) == _parents.end())
      continue;

    std::unordered_set<model *> visited;

    // store the model and the path [start, model] in queue
    using stack_t = std::pair<model *, std::unordered_set<model *>>;
    std::stack<stack_t> stack;
    stack.push({start, {start}});

    while (!stack.empty()) {
      auto [cm, this_path] = stack.top();
      stack.pop();

      visited.insert(cm);

      for (const auto next : _children[cm]) {
        if (visited.find(next) == visited.end()) {
          auto next_path = this_path;
          next_path.insert(next);

          stack.push({next, next_path});
        } else if (start == next) {
          loops.insert(loops.end(), {this_path.begin(), this_path.end()});
        }
      }
    }
  }
  return loops;
}

} // namespace mbd