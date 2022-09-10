#include "connection.hpp"
#include "connection_state.hpp"
#include "data_type.hpp"
#include "model.hpp"
#include "port.hpp"

namespace mbd
{

connection::build_ret_t connection::build(const end_point &from,
                                          const end_point &to)
{
  if (from._model == to._model)
    return {connection_state::ERR_SAME_MODEL, nullptr};

  if (to._port->is_connected())
    return {connection_state::ERR_IN_PORT_CONNECTED, nullptr};

  if (from._port->get_dir() == to._port->get_dir())
    return {connection_state::ERR_SAME_DIRECTION, nullptr};

  if (from._port->get_data_type() != to._port->get_data_type())
    return {connection_state::ERR_DIFF_TYPE, nullptr};

  return {connection_state::CONNECTED, std::make_unique<connection>(from, to)};
}

connection::connection(const end_point &from, const end_point &to)
    : _from(from), _to(to)
{
  // make the connection in the from._model, so that it writes to the to._port
  // if we do this, the connection does not need to be updated
  // the from._model will directly write to the to._port and update it
  _from._model->connect(_from._port, _to._port);

  // set the to._port connected flag
  _to._port->set_connected(true);
}

connection::~connection()
{
  // disconnect from node, so that it stops writing to the to._port
  _from._model->disconnect(_from._port, _to._port);

  // reset the to._port connected flag
  _to._port->set_connected(false);
}

bool connection::operator==(const std::pair<end_point, end_point> &other) const
{
  return this->_from == other.first && this->_to == other.second;
}

/************************************************************************/

end_point::end_point(model *n, port *p) : _model(n), _port(p) {}

end_point::end_point(model *n, std::uint64_t p, port_dir_t dir) : _model(n)
{
  if (dir == port_dir_t::IN)
    _port = &_model->_in_ports[p];
  else
    _port = &_model->_out_ports[p];
}

bool end_point::operator==(const end_point &other) const
{
  return _model == other._model && _port == other._port;
}

} // namespace mbd