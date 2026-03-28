#include "js_view_adapter.hpp"
#include "math_lib.hpp"

#include <cstddef>
#include <unordered_set>

#include <vector>

#include <iostream>
#include <thread>

using namespace mbd;
using namespace mbd::impl;

namespace view_example
{
void js_adapter()
{
  std::size_t port = 6006;

  std::cout << "\n\n"
            << "\t\t View Example: web adapter on port " << port
            << "\n\n";

  const auto math_lib = get_math_lib<double>("Double");
  mbd::view::js_view_adapter view(6006, {math_lib});
  while (1)
  {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}
} // namespace view_example