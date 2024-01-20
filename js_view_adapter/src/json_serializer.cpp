#include "json_serializer.hpp"
#include "data_holder.hpp"
#include "model.hpp"

#include "node.hpp"
#include "uuid.hpp"

#include <cstdint>
#include <nlohmann/json.hpp>

#include <string>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace std {
// type_index is in std namepsace
// and nlohmann::json requires that to_json and from_json
// are defined in the same namespace with the type

using json = nlohmann::json;
std::unordered_map<std::type_index, std::string> type_to_string_map = {
    {typeid(double), "double"},
    {typeid(float), "float"},
    {typeid(int), "int"},
    {typeid(bool), "bool"},
    {typeid(char), "char"},
    {typeid(std::string), "std::string"},
    {typeid(std::int8_t), "std::int8_t"},
    {typeid(std::int16_t), "std::int16_t"},
    {typeid(std::int32_t), "std::int32_t"},
    {typeid(std::int64_t), "std::int64_t"},
    {typeid(std::uint8_t), "std::uint8_t"},
    {typeid(std::uint16_t), "std::uint16_t"},
    {typeid(std::uint32_t), "std::uint32_t"},
    {typeid(std::uint64_t), "std::uint64_t"}};

void to_json(json &j, const std::type_index &idx) {
  j = type_to_string_map.at(idx);
}

} // namespace std

namespace mbd {

/***************************************************************
                            mbd::uuid
****************************************************************/

void to_json(json &j, const mbd::uuid &id) { j = id.to_string(); }

void from_json(const json &j, mbd::uuid &id) {
  id = mbd::uuid(j.get<std::string>());
}

/***************************************************************
                            mbd::data_holder
****************************************************************/

// This maps can be extended to contain user types.
// The user is also required to provide to_json and from_json
// implementaions for their types.

std::unordered_map<std::type_index, h_to_json_func_t> h_to_json_map = {
    {typeid(double), h_to_json_helper<double>},
    {typeid(float), h_to_json_helper<float>},
    {typeid(int), h_to_json_helper<int>},
    {typeid(bool), h_to_json_helper<bool>},
    {typeid(char), h_to_json_helper<char>},
    {typeid(std::string), h_to_json_helper<std::string>},
    {typeid(std::int8_t), h_to_json_helper<std::int8_t>},
    {typeid(std::int16_t), h_to_json_helper<std::int16_t>},
    {typeid(std::int32_t), h_to_json_helper<std::int32_t>},
    {typeid(std::int64_t), h_to_json_helper<std::int64_t>},
    {typeid(std::uint8_t), h_to_json_helper<std::uint8_t>},
    {typeid(std::uint16_t), h_to_json_helper<std::uint16_t>},
    {typeid(std::uint32_t), h_to_json_helper<std::uint32_t>},
    {typeid(std::uint64_t), h_to_json_helper<std::uint64_t>}};

void to_json(json &j, const data_holder &h) {
  const auto type = h.get_data_type();
  if (const auto &it = h_to_json_map.find(type); it != h_to_json_map.end()) {
    it->second(j, h);
  }
}

std::unordered_map<std::type_index, h_from_json_func_t> h_from_json_map = {
    {typeid(double), h_from_json_helper<double>},
    {typeid(float), h_from_json_helper<float>},
    {typeid(int), h_from_json_helper<int>},
    {typeid(bool), h_from_json_helper<bool>},
    {typeid(std::string), h_from_json_helper<std::string>},
    {typeid(std::int8_t), h_from_json_helper<std::int8_t>},
    {typeid(std::int16_t), h_from_json_helper<std::int16_t>},
    {typeid(std::int32_t), h_from_json_helper<std::int32_t>},
    {typeid(std::int64_t), h_from_json_helper<std::int64_t>},
    {typeid(std::uint8_t), h_from_json_helper<std::uint8_t>},
    {typeid(std::uint16_t), h_from_json_helper<std::uint16_t>},
    {typeid(std::uint32_t), h_from_json_helper<std::uint32_t>},
    {typeid(std::uint64_t), h_from_json_helper<std::uint64_t>}};

void from_json(const json &j, data_holder &h) {
  const auto type = h.get_data_type();
  if (const auto &it = h_from_json_map.find(type);
      it != h_from_json_map.end()) {
    it->second(j, h);
  }
}

/***************************************************************
                            mbd::param
****************************************************************/

void to_json(json &j, const mbd::param &p) {
  const data_holder &h = p;

  j = json{{"uuid", p.get_uuid()},
           {"name", p.get_name()},
           {"readonly", p.is_readonly()},
           {"value", h},
           {"type", h.get_data_type()}};
}

void from_json(const json &j, mbd::param &p) {
  // you can only change the value - for now :)
  data_holder &h = p;
  from_json(j, h);
}

/***************************************************************
                            mbd::port
****************************************************************/

void to_json(json &j, mbd::port_dir_t d) {
  j = d == mbd::port_dir_t::IN ? "in" : "out";
}

void to_json(json &j, const mbd::port &p) {
  const data_holder &h = p;

  j = json{{"uuid", p.get_uuid()}, {"name", p.get_name()},
           {"dir", p.get_dir()},   {"connected", p.is_connected()},
           {"value", h},           {"type", h.get_data_type()}};
}

/***************************************************************
                            mbd::node
****************************************************************/

void to_json(json &j, const mbd::node &n) {
  j = json{{"name", n._name},
           {"uuid", n._uuid},
           {"inputs", n._in_ports},
           {"outputs", n._out_ports},
           {"params", n._params}};
}

void from_json(const json &j, mbd::node &n) {
  // you can change only the name in the view
  n._name = j.at("name");

  // but the uuid is needed when loading a design
  n._uuid = j.at("uuid");
}

/***************************************************************
                            mbd::model
****************************************************************/

void to_json(json &j, const mbd::model &m) {
  const mbd::node &n = m;
  j = n;
}

void from_json(const json &j, mbd::model &m) {}

void to_json(json &j, const std::vector<model::ptr_t> &models) {
  json models_array = json::array();
  for (const auto &m : models) {
    models_array.push_back(*m.get());
  }

  j = models_array;
}

/***************************************************************
                            mbd::connection
****************************************************************/

void to_json(json &j, const mbd::connection &c) {
  const auto &[from, to] = c.get_end_points();

  j = json{{"from", *from}, {"to", *to}};
}

void from_json(const json &j, mbd::connection &n) {}

void to_json(json &j, const std::vector<mbd::connection::ptr_t> &conn) {
  json conn_array = json::array();

  for (const auto &c : conn) {
    conn_array.push_back(*c.get());
  }

  j = conn_array;
}

/***************************************************************
                            mbd::end_point
****************************************************************/

void to_json(json &j, const mbd::end_point &p) {
  j = json{{"model", p._model->_uuid}, {"port", p._port->get_uuid()}};
}

void from_json(const json &j, mbd::end_point &p) {}

/***************************************************************
                            mbd::graph
****************************************************************/

void to_json(json &j, const mbd::graph &g) {
  json models_array = g.get_models();

  json conn_array = g.get_connections();

  j = json{{"models", models_array}, {"connections", conn_array}};
}

void from_json(const json &j, mbd::graph &g) {}

/***************************************************************
                            mbd::lib
****************************************************************/

void to_json(json &j, const mbd::lib &l) {
  j = json{{"name", l.get_name()}, {"types", l.get_model_types()}};
}

} // namespace mbd