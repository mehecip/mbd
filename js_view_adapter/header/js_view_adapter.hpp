#include "iview_adapter.hpp"
#include "library.hpp"
#include "node.hpp"

#include <unordered_map>
#include <unordered_set>
#include <utility>

namespace mbd {
namespace view {

class json_ {};

class js_view_adapter : public iview_adapter {
	
public:
  js_view_adapter(std::uint16_t port, const std::unordered_set<mbd::lib> &libs);

  void to_model(mbd::node *n) override;

  void to_view(mbd::node const *n, std::size_t tick_) override;

private:
  json_ to_json(mbd::node const *n);

  json_ to_json(const mbd::lib &n);

  void from_json(const json_ &obj, mbd::node *n);

  void send_async(const json_ &obj);

  void send_blocking(const json_ &obj);

  std::pair<bool, json_> get_updates(const mbd::uuid &id);

  void start_web_server(std::uint16_t port);

  void listen();

  void offer_models(const std::unordered_set<mbd::lib> &libs);

private:
  std::unordered_map<std::string, json_> _updates;
};

} // namespace view
} // namespace mbd