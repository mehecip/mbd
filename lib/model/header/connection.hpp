#pragma once

#include "connection_state.hpp"
#include "data_type.hpp"
#include "log_level.hpp"
#include "port.hpp"

#include <cstddef>
#include <memory>
#include <string>
#include <utility>

namespace mbd
{

class model; // fwd

struct end_point
{
  model *_model;
  port *_port;

  end_point(model *n, port *p);
  end_point(model *n, std::uint64_t p, port_dir_t dir);

  bool operator==(const end_point &other) const;
};

class connection
{
public:
  using ptr_t = std::unique_ptr<connection>;
  using build_ret_t = std::pair<connection_state, ptr_t>;
  using nodes_t = std::pair<model *, model *>;

  static build_ret_t build(const end_point &from, const end_point &to);

  connection(const end_point &from, const end_point &to);
  ~connection();

  bool operator==(const std::pair<end_point, end_point> &other) const;

  nodes_t get_models() const{ return {_from._model, _to._model}; }

private:
  end_point _from, _to;
  i_data_type::ptr_t _data_ptr;
};

} // namespace mbd