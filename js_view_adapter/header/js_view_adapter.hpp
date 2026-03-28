#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "nlohmann/json_fwd.hpp"
#include "uuid.hpp"

namespace mbd {

class lib;
class node;

namespace view {
using json = nlohmann::json;

class js_view_adapter {
public:
    js_view_adapter(std::uint16_t port, const std::vector<mbd::lib>& libs);
    ~js_view_adapter();

    void to_model(mbd::node* n);
    void to_view(mbd::node const* n, std::size_t tick_);

private:
    void send_async(const json& obj);
    void send_blocking(const json& obj);
    std::pair<bool, std::string> get_updates(const mbd::uuid& id);
    void start_web_server(std::uint16_t port);
    void listen();
    void do_accept();
    void do_read();
    void offer_models(const std::vector<mbd::lib>& libs);
    void handle_message(const std::string& message);

    std::unordered_map<std::string, std::string> _updates;
    std::vector<mbd::lib> _libs;

    public:
    class impl;
    std::unique_ptr<impl> pimpl;
};

} // namespace view
} // namespace mbd