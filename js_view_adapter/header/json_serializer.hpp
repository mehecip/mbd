#pragma once

#include "data_holder.hpp"
#include "graph.hpp"
#include "node.hpp"
#include "param.hpp"
#include "port.hpp"
#include "uuid.hpp"

#include "connection.hpp"
#include "connection_state.hpp"

#include "graph.hpp"

#include "library.hpp"

#include <nlohmann/json_fwd.hpp>

#include <string>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace std
{
using json = nlohmann::json;

/***************************************************************
                            std::type_index
****************************************************************/
extern std::unordered_map<std::type_index, std::string> type_to_string_map;
void to_json(json &j, const std::type_index &idx);
} // namespace std

namespace mbd
{

using json = nlohmann::json;


/***************************************************************
                            mbd::uuid
****************************************************************/

void to_json(json &j, const mbd::uuid &id);
void from_json(const json &j, mbd::uuid &id);

/***************************************************************
                            mbd::data_holder
****************************************************************/

using h_to_json_func_t = std::function<void(json &j, const data_holder &)>;
extern std::unordered_map<std::type_index, h_to_json_func_t> h_to_json_map;

template <typename T>
const h_to_json_func_t &h_to_json_helper = [](json &j, const data_holder &h) {
  j = h.template read_data<T>();
};

void to_json(json &j, const data_holder &h);

using h_from_json_func_t = std::function<void(const json &j, data_holder &)>;
extern std::unordered_map<std::type_index, h_from_json_func_t> h_from_json_map;

template <typename T>
const h_from_json_func_t &h_from_json_helper =
    [](const json &j, data_holder &h) { h.template write_data<T>(j); };

void from_json(const json &j, data_holder &n);

/***************************************************************
                            mbd::param
****************************************************************/

void to_json(json &j, const mbd::param &p);
void from_json(const json &j, mbd::param &p);

/***************************************************************
                            mbd::port
****************************************************************/

void to_json(json &j, mbd::port_dir_t d);
void to_json(json &j, const mbd::port &p);

/***************************************************************
                            mbd::node
****************************************************************/

void to_json(json &j, const mbd::node &n);
void from_json(const json &j, mbd::node &n);

/***************************************************************
                            mbd::model
****************************************************************/

void to_json(json &j, const mbd::model &n);
void from_json(const json &j, mbd::model &n);

/***************************************************************
                            mbd::connection
****************************************************************/

void to_json(json &j, const mbd::connection &c);
void from_json(const json &j, mbd::connection &c);

/***************************************************************
                            mbd::end_point
****************************************************************/

void to_json(json &j, const mbd::end_point &p);
void from_json(const json &j, mbd::end_point &p);

/***************************************************************
                            mbd::graph
****************************************************************/

void to_json(json &j, const mbd::graph &g);
void from_json(const json &j, mbd::graph &g);


/***************************************************************
                            mbd::lib
****************************************************************/

void to_json(json &j, const mbd::lib &l);



/***************************************************************
                            Helper functions
****************************************************************/

/***************************************************************
register_type allows users to register a custom parameter
type after implementing to_json() and from_json() functions. 
In this way, the serializer will be able to convert the
mbd::param<user_type> to json and back.

e.g.: 

struct my_pod {bool flag; int value; string id;}
void to_json(...) {...}
void from_json(...) {...}

// in e.g.  the model model constructor 
...
mbd::register_type<my_pod>("my_pod_type");

****************************************************************/
template <typename T> 
void register_type(const std::string &type_name) {
  h_to_json_map.emplace(typeid(T), h_to_json_helper<T>);
  h_from_json_map.emplace(typeid(T), h_from_json_helper<T>);
  std::type_to_string_map.emplace(typeid(T), type_name);
}


} // namespace mbd