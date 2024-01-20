#include "connection.hpp"
#include "connection_state.hpp"
#include "data_type.hpp"
#include "model.hpp"
#include "port.hpp"

namespace mbd {

connection::build_ret_t connection::build(const end_point &from,
                                          const end_point &to) {
  if (from._model == to._model)
    return {connection_state::ERR_SAME_MODEL, nullptr};

  if (to._port->is_connected())
    return {connection_state::ERR_IN_PORT_CONNECTED, nullptr};

  if (from._port->get_dir() == to._port->get_dir())
    return {connection_state::ERR_SAME_DIRECTION, nullptr};

  if (from._port->get_data_type() != to._port->get_data_type())
    return {connection_state::ERR_DIFF_TYPE, nullptr};

  static constexpr auto state = connection_state::CONNECTED;
  if (from._port->get_dir() == port_dir_t::OUT)
    return {state, std::make_unique<connection>(from, to)};

  return {state, std::make_unique<connection>(to, from)};
}

connection::connection(const end_point &from, const end_point &to)
    : _from(from), _to(to) {
  // set the to._port connected flag
  _to._port->set_connected(true);

  // Connecting 2 ports boils down to making sure that both point to the same
  // memory location. By doing this, the connection itself does not need to be
  // updated, nor the data moved between the ports from the outside.

  _data_ptr = nullptr;

  // store the input port data pointer in _data_ptr
  std::swap(_data_ptr, _to._port->_data);

  // set the input port data pointer to the output port data pointer
  _to._port->_data = _from._port->_data;
}

connection::~connection() {
  // reset the to._port connected flag
  _to._port->set_connected(false);

  // set the input port data pointer to the old input port data pointer
  _to._port->_data = nullptr;
  std::swap(_data_ptr, _to._port->_data);
}

bool connection::operator==(
    const std::pair<end_point, end_point> &other) const {
  return this->_from == other.first && this->_to == other.second;
}

connection::models_t connection::get_models() const {
  return {_from._model, _to._model};
}

connection::end_points_t connection::get_end_points() const {
  return {&_from, &_to};
}

/************************************************************************/

end_point::end_point(model *n, port *p) : _model(n), _port(p) {}

end_point::end_point(model *n, std::uint64_t p, port_dir_t dir) : _model(n) {
  if (dir == port_dir_t::IN)
    _port = &_model->_in_ports[p];
  else
    _port = &_model->_out_ports[p];
}

bool end_point::operator==(const end_point &other) const {
  return _model == other._model && _port == other._port;
}

} // namespace mbd