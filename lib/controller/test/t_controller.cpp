
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
      _ctrl.add_model(lib.get_name(), type);
    }
  }

  void addModels(const mbd::lib &lib, const std::vector<std::string>& models)
  {
    _ctrl.add_library(lib);
    for (const auto &type : models)
    {
      _ctrl.add_model(lib.get_name(), type);
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
    [](const auto &actual_models,
       const std::vector<std::vector<std::string>> &expected_names) {

      std::vector<std::vector<std::string>> actual_names;
      for (const auto &models : actual_models)
      {
        std::vector<std::string> names;
        for (const auto &model : models)
        {
          names.push_back(model->get_name());
        }
        std::sort(names.begin(), names.end());
        actual_names.push_back(names);
      }
      
      EXPECT_EQ(actual_names, expected_names);
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
    {"gain"},
    {"sink"}
  });

  fx._ctrl.disconnect("constant source", 0, "gain", 0);

  // the source is not connected to anything, so it should be the only model to be actually executed
  order = fx._ctrl.execution_order();
  check_execution_order(order, {
    {"constant source"}
  });

  // now connect the source to the gain again
  fx._ctrl.connect("constant source", 0, "gain", 0);
  EXPECT_EQ(flag, true) << "Re-Connect should work.";

  order = fx._ctrl.execution_order();
  check_execution_order(order, {
    {"constant source"},
    {"gain"}
  });

  // now connect the gain to the sink again
  flag = fx._ctrl.connect("gain", 0, "sink", 0);
  EXPECT_EQ(flag, true) << "Re-Connect should work.";

  order = fx._ctrl.execution_order();
  check_execution_order(order, {
    {"constant source"},
    {"gain"},
    {"sink"}
  });
}


TEST(ControllerTest, tExecutionOrder_MultipleSources)
{
  auto lib = get_math_lib<double>("double");

  ControllerFixture fx;
  fx.addModels(lib, {"const_src", "gain", "sink", "lin_src", "double_to", "sum", "delay"});

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

  auto order = fx._ctrl.execution_order();
  check_execution_order(order, {
    {"constant source", "linear source", "unit delay"},
    {"type convertor" },
    {"add"},
    {"gain"},
    {"sink"}
  });
}

TEST(ControllerTest, tExecutionOrder_UnconnectedSources)
{
  auto lib = get_math_lib<double>("double");

  ControllerFixture fx;
  fx.addAllModels(lib);

  auto order = fx._ctrl.execution_order();
  check_execution_order(order, {
    {"constant source", "linear source", "unit delay"}
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

  auto actual = sink->read();
  EXPECT_EQ(actual, exp) << "Sink should contain the expected values after running the controller.";

  fx._ctrl.disconnect("constant source", 0, "gain", 0);
  fx._ctrl.run(2);

  actual = sink->read();
  EXPECT_EQ(actual, exp) << "Sink should not have any other values since the source is disconnected.";

  // reconnect the source and run the controller again
  flag = fx._ctrl.connect("constant source", 0, "gain", 0);
  EXPECT_EQ(flag, true) << "Connect should work.";
  fx._ctrl.run(2);

  actual = sink->read();
  EXPECT_EQ(actual, exp) << "Sink should not have any other values since the gain is disconnected.";

  // reconnect the gain to sink and run the controller again
  flag = fx._ctrl.connect("gain", 0, "sink", 0);
  EXPECT_EQ(flag, true) << "Connect should work.";

  fx._ctrl.run(2);

  exp = {0, 0, -15, -15, -15, 30, 30, 30};
  actual = sink->read();
  EXPECT_EQ(actual, exp) << "Sink should contain the expected values after reconnecting the source and running the controller again.";

}