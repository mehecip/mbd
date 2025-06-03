
#include "gtest/gtest.h"
#include <csignal>
#include <cstddef>
#include <cstdint>
#include <gtest/gtest.h>
#include <memory>
#include <optional>
#include <signal.h>
#include <string>
#include <vector>

#include "controller.hpp"
#include "library_fixtures/math_lib.hpp"
#include "model.hpp"

using namespace mbd;
using namespace tst;

class ControllerFixture
{

public:
  ControllerFixture() : _ctrl(ControllerFixture::message_callback) {}

  void addAllModels(const mbd::lib &lib)
  {
    _ctrl.add_library(lib);
    for (const auto &type : lib.get_model_types())
    {
      _ctrl.add_model(lib.get_name(), type) != nullptr;
    }
  }

  void addModels(const mbd::lib &lib, const std::vector<std::string>& models)
  {
    _ctrl.add_library(lib);
    for (const auto &type : models)
    {
      _ctrl.add_model(lib.get_name(), type) != nullptr;
    }
  }

  mbd::controller _ctrl;

private:
  static void message_callback(log_level lvl, const std::string &msg)
  {
    std::cout << level_info(lvl) << ": " << msg << "\n";
  }
};

const auto &check_execution_order =
    [](const auto &actual,
       const std::vector<std::vector<std::string>> &expected) {

      ASSERT_EQ(actual.size(), expected.size());

      std::size_t idx = 0;
      for (auto models : actual) {

        EXPECT_EQ(models.size(), expected[idx].size())
            << "The number of models in priority vector " << idx
            << " does not match the expected number.";

        for (std::size_t i = 0; i < models.size(); ++i)
        {
          EXPECT_EQ(models[i]->get_name(), expected[idx][i])
              << "Model at index " << i << " in priority vector " << idx
              << " does not match the expected model.";
        }
        ++idx;
      }
    };

TEST(ControllerTest, tConnect)
{
  ControllerFixture fx;
  fx.addAllModels(get_math_lib<double>("double"));

  auto flag = fx._ctrl.connect("constant source", 0, "add", 0);
  EXPECT_EQ(flag, true) << "Connect should work.";

  flag = fx._ctrl.connect("linear source", 0, "type convertor", 0);
  EXPECT_EQ(flag, true) << "Connect should work.";

  flag = fx._ctrl.connect("type convertor", 0, "add", 1);
  EXPECT_EQ(flag, true) << "Connect should work.";

  flag = fx._ctrl.connect("add", 0, "unit delay", 0);
  EXPECT_EQ(flag, true) << "Connect should work.";

  flag = fx._ctrl.connect("unit delay", 0, "gain", 0);
  EXPECT_EQ(flag, true) << "Connect should work.";

  flag = fx._ctrl.connect("gain", 0, "sink", 0);
  EXPECT_EQ(flag, true) << "Connect should work.";
}


TEST(ControllerTest, tExecutionOrder)
{
  auto lib = get_math_lib<double>("double");

  ControllerFixture fx;
  fx.addModels(lib, {"const_src", "gain", "sink"});
  /*
  
  | Source | -> | Gain | -> | Sink |
  */

  auto flag = fx._ctrl.connect("constant source", 0, "gain", 0);
  EXPECT_EQ(flag, true) << "Connect should work.";

  flag = fx._ctrl.connect("gain", 0, "sink", 0);
  EXPECT_EQ(flag, true) << "Connect should work.";

  auto order = fx._ctrl.execution_order();
  check_execution_order(order, {
    {"constant source"},
    {"gain"}, {"sink"}
  });

}

TEST(ControllerTest, tRun)
{
  auto lib = get_math_lib<int>("int");

  ControllerFixture fx;
  fx.addModels(lib, {"const_src", "gain", "sink"});

  auto flag = fx._ctrl.connect("constant source", 0, "gain", 0);
  EXPECT_EQ(flag, true) << "Connect should work.";

  flag = fx._ctrl.connect("gain", 0, "sink", 0);
  EXPECT_EQ(flag, true) << "Connect should work.";

  /*
  | Source | -> | Gain | -> | Sink |
  */

  auto csrc = fx._ctrl.get<tst::const_source<int>>("constant source");
  auto gain = fx._ctrl.get<tst::gain<int>>("gain");
  auto sink = fx._ctrl.get<tst::sink<int>>("sink");

  csrc->set_value(15);
  csrc->set_init_val(0);
  csrc->set_step_tick(3);

  gain->set_value(-1);

  EXPECT_EQ(sink->read(), std::vector<int>{}) << "Sink should have no values before running the controller.";

  // run the controller for 2 ticks -> sink should contain {0, 0}
  fx._ctrl.run(2);

  // run the controller for 1 more tick -> sink should contain {0, 0, -15}
  fx._ctrl.run(1);

  // run the controller for 2 more ticks -> sink should contain {0, 0, -15, -15, -15}
  fx._ctrl.run(2);
  
  gain->set_value(2);     
  // run the controller for 1 more tick -> sink should contain {0, 0, -15, -15, -15, 30}
  fx._ctrl.run(1);

  std::vector<int> exp {0, 0, -15, -15, -15, 30};

  const auto& actual = sink->read();
  EXPECT_EQ(actual, exp) << "Sink should contain the expected values after running the controller.";
}