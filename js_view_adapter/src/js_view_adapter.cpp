#include "js_view_adapter.hpp"
#include "library.hpp"

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>

#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <mutex>
#include <memory>
#include <thread>

#include "json_serializer.hpp"
#include <nlohmann/json.hpp>

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

namespace mbd {
namespace view {

class js_view_adapter::impl
{
public:
  net::io_context ioc;
  websocket::stream<tcp::socket> ws{ioc};
  tcp::acceptor acceptor{ioc};
  beast::flat_buffer buffer;
  std::mutex connection_mutex;
  bool has_client{false};
  std::thread io_thread;

  ~impl();
};
js_view_adapter::impl::~impl()
{
  if (io_thread.joinable())
  {
    ioc.stop();
    io_thread.join();
  }
}

js_view_adapter::js_view_adapter(std::uint16_t port,
                               const std::vector<mbd::lib> &libs) 
    : pimpl(std::make_unique<js_view_adapter::impl>()), _libs(libs) {
    start_web_server(port);
    listen();
    offer_models(_libs);
}

js_view_adapter::~js_view_adapter() = default;

void js_view_adapter::start_web_server(std::uint16_t port) {
    try {
        auto endpoint = tcp::endpoint{tcp::v4(), port};
        pimpl->acceptor.open(endpoint.protocol());
        pimpl->acceptor.set_option(net::socket_base::reuse_address(true));
        pimpl->acceptor.bind(endpoint);
        pimpl->acceptor.listen(net::socket_base::max_listen_connections);
        
        do_accept();
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}

void js_view_adapter::do_accept() {
    pimpl->acceptor.async_accept(
        pimpl->ws.next_layer(),
        [this](beast::error_code ec) {
            if (!ec) {
                std::lock_guard<std::mutex> lock(pimpl->connection_mutex);
                pimpl->has_client = true;
                std::cout << "Client connected\n";
                
                // Accept the websocket handshake
                pimpl->ws.async_accept(
                    [this](beast::error_code ec) {
                        offer_models(_libs);
                        if (!ec) {
                            do_read();
                        }
                    });
            }
            
            // Accept another connection
            do_accept();
        });
}

void js_view_adapter::do_read() {
    pimpl->ws.async_read(
        pimpl->buffer,
        [this](beast::error_code ec, std::size_t bytes_transferred) {
            if (!ec) {
                // Handle the message
                std::string message = beast::buffers_to_string(pimpl->buffer.data());
                handle_message(message);
                pimpl->buffer.consume(pimpl->buffer.size());
                
                // Continue reading
                do_read();
            } else {
                std::lock_guard<std::mutex> lock(pimpl->connection_mutex);
                pimpl->has_client = false;
                std::cout << "Client disconnected\n";
                 do_accept();
            }
        });
}

void js_view_adapter::listen() {
    pimpl->io_thread = std::thread([this]() {
        try {
            pimpl->ioc.run();
        } catch (const std::exception& e) {
            std::cerr << "Exception in WebSocket thread: " << e.what() << std::endl;
        }
    });
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

void js_view_adapter::send_async(const json& obj) {
    try {
        std::lock_guard<std::mutex> lock(pimpl->connection_mutex);
        if (pimpl->has_client) {
            pimpl->ws.async_write(
                net::buffer(obj.dump()),
                [](beast::error_code ec, std::size_t bytes_transferred) {
                    if (ec) {
                        std::cerr << "Error sending message: " << ec.message() << std::endl;
                    }
                });
        }
    } catch (const std::exception& e) {
        std::cerr << "Error sending message: " << e.what() << std::endl;
    }
}

void js_view_adapter::send_blocking(const json& obj) {
    try {
        std::lock_guard<std::mutex> lock(pimpl->connection_mutex);
        if (pimpl->has_client) {
            pimpl->ws.write(net::buffer(obj.dump()));
        }
    } catch (const std::exception& e) {
        std::cerr << "Error sending message: " << e.what() << std::endl;
    }
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

void js_view_adapter::handle_message(const std::string& message) {
    try {
        auto j = json::parse(message);
        if (j.contains("uuid")) {
            std::string uuid = j["uuid"];
            _updates[uuid] = message;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error parsing message: " << e.what() << std::endl;
    }
}

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