#include "js_view_adapter.hpp"
#include "library.hpp"

#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "json_serializer.hpp"
#include <nlohmann/json.hpp>

namespace mbd {
namespace view {

js_view_adapter::js_view_adapter(std::uint16_t port,
                                 const std::vector<mbd::lib> &libs) {
  start_web_server(port);
  listen();
  offer_models(libs);
}

void js_view_adapter::to_model(mbd::node *n) {
  const auto &[out_dated, update] = get_updates(n->_uuid);
  if (out_dated) {
    *n = json{update};
  }
}

void js_view_adapter::to_view(mbd::node const *n, std::size_t tick_) {
  send_async(*n); // or send_blocking
}

void js_view_adapter::send_async(const json &obj) {
  std::cout << obj;
  // send to websocket
}

void js_view_adapter::send_blocking(const json &obj) {
  // send to websocket
}

// blocking
std::pair<bool, std::string> js_view_adapter::get_updates(const mbd::uuid &id) {
  // the updates are received async in the background
  // and get_updates just checks if there are any for the id
  const auto &it = _updates.find(id.to_string());
  if (it != _updates.end()) {
    std::pair<bool, std::string> ret{true, it->second};
    _updates.erase(it);

    return ret;
  }

  return {false, {}};
}

void js_view_adapter::start_web_server(std::uint16_t port) {}

void js_view_adapter::listen() {}

void js_view_adapter::offer_models(const std::vector<mbd::lib> &libs) {

  send_async(libs);

  /* lib serialization
  {
      "name": "my lib",
      "models" : [
          {
              "uuid": "5fd40d82-e711-445b-b832-c5c8d88a47e8",
              "name" : "gain",

              "in_ports": [
                  {
                      "idx":0,
                      "type": "double",
                      "init_val": 0.0
                  }
              ],

              "out_ports": [
                  {
                      "idx":0,
                      "type": "double",
                      "init_val": 0.0,
                      "val": 0.0
                  },
              ],
              "params": [
                  {
                      "idx":0,
                      "type": "double",
                      "val": 3.0,
                      "name": "Gain"
                  },
              ],

              "is_source": false
          },
      ]
  }

  */
}
} // namespace view
} // namespace mbd