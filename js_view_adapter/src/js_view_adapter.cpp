#include "js_view_adapter.hpp"
#include "library.hpp"

#include <utility>
#include <unordered_set>
#include <unordered_map>

namespace mbd {
namespace view {

js_view_adapter::js_view_adapter(std::uint16_t port,
                                 const std::unordered_set<mbd::lib> &libs) {
  start_web_server(port);
  listen();
  offer_models(libs);
}

void js_view_adapter::to_model(mbd::node *n) {
  const auto &[needs_update, update] = get_updates(n->_uuid);
  if (needs_update) {
    from_json(update, n);
  }
}

void js_view_adapter::to_view(mbd::node const *n, std::size_t tick_) {
  json_ obj = to_json(n);
  send_async(obj); // or send_blocking
}

json_ js_view_adapter::to_json(mbd::node const *n) { return json_(); }

void js_view_adapter::from_json(const json_ &obj, mbd::node *n) {}

json_ js_view_adapter::to_json(const mbd::lib &n) { return json_(); }

void js_view_adapter::send_async(const json_ &obj) {
  // send to websocket
}

void js_view_adapter::send_blocking(const json_ &obj) {
  // send to websocket
}
// blocking
std::pair<bool, json_> js_view_adapter::get_updates(const mbd::uuid &id) {
  // the updates are received async in the background
  // and get_updates just checks if there are any for the id
  const auto &it = _updates.find(id.to_string());
  if (it != _updates.end()) {
    std::pair<bool, json_> ret{true, it->second};
    _updates.erase(it);

    return ret;
  }

  return {false, json_()};
}

void js_view_adapter::start_web_server(std::uint16_t port) {}

void js_view_adapter::listen() {}

void js_view_adapter::offer_models(const std::unordered_set<mbd::lib> &libs) {
  json_ obj;

  for (const auto &lib : libs) {
    obj = to_json(lib);
  }

  send_async(obj);

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

              "is_feedthrough": false
          },
      ]
  }

  */
}
} // namespace view
} // namespace mbd