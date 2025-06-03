
#include <future>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "controller.hpp"
#include "library.hpp"
#include "model.hpp"
#include "msg_dispatcher.hpp"

namespace mbd
{
controller::controller(const msg_callback_f &f) : _callback_f(f) {}

void controller::add_library(const lib &l) { _libs.emplace(l.get_name(), l); }

model *controller::add_model(const std::string &libname,
                             const std::string &modeltype)
{
  const auto &it = _libs.find(libname);
  if (it == _libs.end())
    return nullptr;

  auto mdl = it->second.build_model(modeltype);
  if (mdl == nullptr)
    return nullptr;

  model *ptr = mdl.get();

  mdl->add_msg_callback(_callback_f);
  _g.push_model(std::move(mdl));

  return ptr;
}

bool controller::connect(const std::string &out_model, std::uint64_t out_idx,
                         const std::string &in_model, std::uint64_t in_idx)
{
  return _g.connect(out_model, out_idx, in_model, in_idx);
}

bool controller::disconnect(const std::string &out_model, std::uint64_t out_idx,
                            const std::string &in_model, std::uint64_t in_idx)
{
  return _g.disconnect(out_model, out_idx, in_model, in_idx);
}

std::vector<model_vec_t> controller::execution_order()
{
  return _g.execution_order();
}

void controller::run(std::uint64_t ticks)
{
  // always get the execution order from the graph
  // this is to ensure that the order is up to date
  const auto &order = _g.execution_order();
  log_prio(order);

  std::uint64_t max_ticks = _current_tick + ticks;
  for (; _current_tick < max_ticks; ++_current_tick)
  {
    // call update in the order of the prios
    for (const auto &models : order)
    {
      for (auto model : models)
        model->update(_current_tick);
    }
  }
}

void controller::run_async(std::uint64_t ticks)
{
  const auto &order = _g.execution_order();
  log_prio(order);

  std::uint64_t max_ticks = _current_tick + ticks;
  for (;_current_tick < max_ticks; ++_current_tick)
  {
    // call update in the order of the prios
    for (const auto &models : order)
    {
      std::vector<std::future<void>> futures;

      // launch update() for all the models with the current priority
      for (const auto &model : models)
        futures.push_back(
            std::async(std::launch::async, &model::update, model, _current_tick));

      // wait for all models with the current priority to finish updating
      for (const auto &f : futures)
        f.wait();
    }
  }
}

void controller::log_prio(const std::vector<std::vector<model *>> &v)
{
  std::ostringstream ss;
  for (std::uint64_t idx = 0; idx < v.size(); ++idx)
  {
    ss << "Update priority " << idx << ": ";
    for (const auto &model : v[idx])
      ss << " [" << model->get_name() << "]";
    ss << "\n\t";
  }

  _callback_f(log_level::DEBUG, ss.str());
}

std::size_t controller::find_algebraic_loops()
{
  const auto &loops = _g.algebraic_loops();
  if (loops.size() != 0)
  {
    std::ostringstream ss;
    ss << loops.size() << " algebraic loop/s found: \n";
    for (const auto &l : loops)
    {
      for (const auto &m : l)
        ss << "[" << m->get_name() << "] ";
      ss << "\n";
    }

    _callback_f(log_level::ERROR, ss.str());
  }

  return loops.size();
}

} // namespace mbd
