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

class node; // fwd

struct end_point
{
  node *_node;
  port *_port;

  end_point(node *n, port *p);
  end_point(node *n, std::uint64_t p, port_dir_t dir);

  bool operator==(const end_point &other) const;
};

class connection
{
public:
  using ptr_t = std::unique_ptr<connection>;
  using build_ret_t = std::pair<connection_state, ptr_t>;

  static build_ret_t build(const end_point &from, const end_point &to);

  connection(const end_point &from, const end_point &to);
  ~connection();

  bool operator==(const std::pair<end_point, end_point> &other) const;

private:
  end_point _from, _to;
};

} // namespace mbd