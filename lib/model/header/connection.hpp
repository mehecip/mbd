#pragma once

#include "connection_state.hpp"
#include "data_type.hpp"

#include <memory>
#include <optional>
#include <utility>

namespace mbd
{

class model; // fwd
class port; // fwd
enum class port_dir_t : bool; // fwd

struct end_point
{
  using build_ret_t = std::pair<connection_state, std::optional<end_point>>;//std::pair<connection_state, std::unique_ptr<end_point>>;

  model * _model;
  port * _port;

  end_point(model * n, port *p);
  end_point(model * n, std::uint64_t pidx, port_dir_t dir);
  static build_ret_t build(model * n, std::uint64_t p, port_dir_t dir);

  bool operator==(const end_point &other) const;
};

class connection
{
public:
  using ptr_t = std::unique_ptr<connection>;
  using build_ret_t = std::pair<connection_state, ptr_t>;
  using models_t = std::pair<model *, model *>;
  using end_points_t = std::pair<const end_point*, const end_point*>;

  static build_ret_t build(const end_point &from, const end_point &to);

  connection(const end_point &from, const end_point &to);
  ~connection();

  bool operator==(const std::pair<end_point, end_point> &other) const;

  models_t get_models() const;
  
  end_points_t get_end_points() const;

private:
  end_point _from, _to;
  i_data_type::ptr_t _data_ptr;
};

} // namespace mbd