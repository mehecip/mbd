
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

model* controller::add_model(const std::string &libname,
                           const std::string &modelname)
{
  const auto &it = _libs.find(libname);
  if (it == _libs.end())
    return nullptr;

  auto mdl = it->second.build_model(modelname);
  if (mdl == nullptr)
    return nullptr;

  model* ptr = mdl.get();

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

void controller::run(std::uint64_t ticks)
{
  const auto &order = _g.template execution_order();
  log_prio(order);

  for (std::uint64_t t = 0; t < ticks; ++t)
  {
    // call update in the order of the prios
    for (const auto &models : order)
    {
      for (auto model : models)
        model->update(t);
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

  _callback_f(log_level::INFO, ss.str());
}

std::size_t controller::find_algebraic_loops()
{
  const auto &loops = _g.template algebraic_loops();
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
