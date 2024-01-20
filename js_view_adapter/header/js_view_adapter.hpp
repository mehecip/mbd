#include "data_holder.hpp"
#include "iview_adapter.hpp"
#include "json_serializer.hpp"
#include "library.hpp"
#include "node.hpp"

#include <nlohmann/json_fwd.hpp>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace mbd
{
namespace view
{

using json = nlohmann::json;

class js_view_adapter : public iview_adapter
{

public:
  js_view_adapter(std::uint16_t port, const std::vector<mbd::lib> &libs);

  void to_model(mbd::node *n) override;

  void to_view(mbd::node const *n, std::size_t tick_) override;

  template <typename T>
  void register_param(const std::string& type_name) const;

private:
  void send_async(const json &obj);

  void send_blocking(const json &obj);

  std::pair<bool, std::string> get_updates(const mbd::uuid &id);

  void start_web_server(std::uint16_t port);

  void listen();

  void offer_models(const std::vector<mbd::lib> &libs);

private:
  std::unordered_map<std::string, std::string> _updates;
};

template <typename T>
void js_view_adapter::register_param(const std::string& type_name) const
{
  h_to_json_map.emplace(typeid(T), h_to_json_helper<T>);
  h_from_json_map.emplace(typeid(T), h_from_json_helper<T>);
  std::type_to_string_map.emplace(typeid(T), type_name);
}

} // namespace view
} // namespace mbd