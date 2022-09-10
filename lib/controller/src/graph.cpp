#include "graph.hpp"
#include "model.hpp"

namespace mbd
{
void graph::push_model(model::ptr_t &&m)
{
  _models.push_back(std::move(m));

  _dirty_algeb_cache = true;
  _dirty_order_cache = true;
}

bool graph::connect(const std::string &out_model, std::uint64_t out_idx,
                    const std::string &in_model, std::uint64_t in_idx)
{
  const auto out = find_model(out_model);
  if (out == nullptr)
    return false;

  const auto in = find_model(in_model);
  if (in == nullptr)
    return false;

  const end_point from{out, out_idx, port_dir_t::OUT};
  const end_point to{in, in_idx, port_dir_t::IN};

  auto [state, conn] = connection::build(from, to);
  if (connection_state::CONNECTED != state)
    return false;

  _connections.push_back(std::move(conn));

  _dirty_algeb_cache = true;
  _dirty_order_cache = true;

  return true;
}

bool graph::disconnect(const std::string &out_model, std::uint64_t out_idx,
                       const std::string &in_model, std::uint64_t in_idx)
{
  const auto out = find_model(out_model);
  if (out == nullptr)
    return false;

  const auto in = find_model(in_model);
  if (in == nullptr)
    return false;

  const end_point from{out, out_idx, port_dir_t::OUT};
  const end_point to{in, in_idx, port_dir_t::IN};

  // make a dummy connection
  std::pair<end_point, end_point> conn{from, to};

  _connections.erase(
      std::find_if(_connections.cbegin(), _connections.cend(),
                   [&conn](const auto &c) { return *c.get() == conn; }),
      _connections.cend());

  _dirty_algeb_cache = true;
  _dirty_order_cache = true;
  return true;
}

model *graph::find_model(const std::string &name)
{
  // search the cache
  const auto &it = _models_cache.find(name);
  if (it != _models_cache.end())
  {
    return it->second;
  }

  // search the vector
  const auto v_it =
      std::find_if(_models.cbegin(), _models.cend(),
                   [&name](const auto &m) { return m->get_name() == name; });

  if (v_it == _models.cend())
  {
    return nullptr;
  }

  // Update the cache
  auto &mdl = *v_it;
  _models_cache.emplace(mdl->get_name(), mdl.get());

  return mdl.get();
}
} // namespace mbd