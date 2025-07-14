
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
#include "controller_helper.hpp"
#include "library_fixtures/math_lib.hpp"
#include "model.hpp"

using namespace mbd;
using namespace tst;


TEST(ControllerTest, tConnect)
{
  ControllerHelper fx;

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

TEST(ControllerTest, tReconnect)
{
  ControllerHelper fx;

  fx.addAllModels(get_math_lib<double>("double"));

  auto flag = fx._ctrl.connect("constant source", 0, "add", 0);
  EXPECT_EQ(flag, true) << "Connect should work.";

  flag = fx._ctrl.disconnect("constant source", 0, "add", 0);
  EXPECT_EQ(flag, true) << "Disconnect should work.";

  flag = fx._ctrl.connect("constant source", 0, "add", 0);
  EXPECT_EQ(flag, true) << "Reconnect should work.";

  flag = fx._ctrl.connect("constant source", 0, "add", 0);
  EXPECT_EQ(flag, false) << "Connecting allready connected models should fail.";
}

TEST(ControllerTest, tExecutionOrder)
{
  auto lib = get_math_lib<double>("double");

  ControllerHelper fx;

  fx.addModels(lib, {"const_src", "gain", "sink"});
  /*

  | Source | -> | Gain | -> | Sink |
  */

  auto flag = fx._ctrl.connect("constant source", 0, "gain", 0);
  EXPECT_EQ(flag, true) << "Connect should work.";

  flag = fx._ctrl.connect("gain", 0, "sink", 0);
  EXPECT_EQ(flag, true) << "Connect should work.";

  auto order = fx._ctrl.execution_order();
  check_execution_order(order, {{"constant source"}, {"gain"}, {"sink"}});

  fx._ctrl.disconnect("constant source", 0, "gain", 0);

  // the source is not connected to anything, so it should be the only model to
  // be actually executed
  order = fx._ctrl.execution_order();
  check_execution_order(order, {{"constant source"}});

  // now connect the source to the gain again
  fx._ctrl.connect("constant source", 0, "gain", 0);
  EXPECT_EQ(flag, true) << "Re-Connect should work.";

  order = fx._ctrl.execution_order();
  check_execution_order(order, {{"constant source"}, {"gain"}, {"sink"}});
}

TEST(ControllerTest, tExecutionOrder_MultipleSources)
{
  auto lib = get_math_lib<double>("double");

  ControllerHelper fx;

  fx.addModels(lib, {"const_src", "gain", "sink", "lin_src", "double_to", "sum",
                     "delay"});

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
  check_execution_order(order,
                        {{"constant source", "linear source", "unit delay"},
                         {"type convertor"},
                         {"add"},
                         {"gain"},
                         {"sink"}});
}

TEST(ControllerTest, tExecutionOrder_UnconnectedSources)
{
  auto lib = get_math_lib<double>("double");

  ControllerHelper fx;

  fx.addAllModels(lib);

  auto order = fx._ctrl.execution_order();
  check_execution_order(order,
                        {{"constant source", "linear source", "unit delay"}});
}

TEST(ControllerTest, tRun)
{
  auto lib = get_math_lib<int>("int");

  ControllerHelper fx;

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

  EXPECT_EQ(sink->read(), std::vector<int>{})
      << "Sink should have no values before running the controller.";

  // run the controller for 2 ticks -> sink should contain {0, 0}
  fx._ctrl.run(2);

  // run the controller for 1 more tick -> sink should contain {0, 0, -15}
  fx._ctrl.run(1);

  // run the controller for 2 more ticks -> sink should contain {0, 0, -15, -15,
  // -15}
  fx._ctrl.run(2);

  gain->set_value(2);
  // run the controller for 1 more tick -> sink should contain {0, 0, -15, -15,
  // -15, 30}
  fx._ctrl.run(1);

  std::vector<int> exp{0, 0, -15, -15, -15, 30};

  auto actual = sink->read();
  EXPECT_EQ(actual, exp) << "Sink should contain the expected values after "
                            "running the controller.";

  fx._ctrl.disconnect("constant source", 0, "gain", 0);
  fx._ctrl.run(2);

  actual = sink->read();
  EXPECT_EQ(actual, exp) << "Sink should not have any other values since the "
                            "source is disconnected.";

  // reconnect the source and run the controller again
  flag = fx._ctrl.connect("constant source", 0, "gain", 0);
  EXPECT_EQ(flag, true) << "Connect should work.";
  fx._ctrl.run(2);

  exp = {0, 0, -15, -15, -15, 30, 30, 30};
  actual = sink->read();
  EXPECT_EQ(actual, exp) << "Sink should contain the expected values after "
                            "running the controller.";
}

TEST(ControllerTest, tAlgebraicLoop)
{

  ControllerHelper fx;

  auto lib_name = "MathLib - double";
  auto lib = get_math_lib<double>("double");
  fx._ctrl.add_library(lib);

  auto delay1 = fx._ctrl.add_model(lib_name, "delay");
  delay1->set_name("Delay 1");

  auto delay2 = fx._ctrl.add_model(lib_name, "delay");
  delay2->set_name("Delay 2");

  auto sum1 = fx._ctrl.add_model(lib_name, "sum");
  sum1->set_name("Sum 1");

  auto sum2 = fx._ctrl.add_model(lib_name, "sum");
  sum2->set_name("Sum 2");

  bool flag = fx._ctrl.connect("Delay 1", 0, "Sum 1", 0);
  EXPECT_EQ(flag, true) << "Connect should work.";

  flag = fx._ctrl.connect("Sum 1", 0, "Delay 1", 0);
  EXPECT_EQ(flag, true) << "Connect should work.";

  flag = fx._ctrl.connect("Sum 1", 0, "Sum 2", 0);
  EXPECT_EQ(flag, true) << "Connect should work.";

  flag = fx._ctrl.connect("Delay 2", 0, "Sum 2", 1);
  EXPECT_EQ(flag, true) << "Connect should work.";

  flag = fx._ctrl.connect("Sum 2", 0, "Sum 1", 1);
  EXPECT_EQ(flag, true) << "Connect should work.";

  flag = fx._ctrl.connect("Sum 2", 0, "Delay 2", 0);
  EXPECT_EQ(flag, true) << "Connect should work.";

  check_algebraic_loops(fx, {{"Sum 1", "Sum 2"}});

  EXPECT_EQ(fx._log[mbd::log_level::ERROR].size(), 1)
      << "There should be one ERROR message for the algebraic loop.";

  EXPECT_EQ(fx._log[mbd::log_level::ERROR].front(),
            "1 algebraic loop/s found: \n[Sum 1] [Sum 2] \n")
      << "Message should match the expected algebraic loop message.";

  auto order = fx._ctrl.execution_order();
  check_execution_order(
      order,
      {}); // execution order should be empty since there is an algebraic loop
}

TEST(ControllerTest, tAlgebraicLoops)
{

  ControllerHelper fx;

  auto lib_name = "MathLib - double";
  auto lib = get_math_lib<double>("double");
  fx._ctrl.add_library(lib);

  auto delay1 = fx._ctrl.add_model(lib_name, "delay");
  delay1->set_name("Delay 1");

  auto sum1 = fx._ctrl.add_model(lib_name, "sum");
  sum1->set_name("Sum 1");

  auto sum2 = fx._ctrl.add_model(lib_name, "sum");
  sum2->set_name("Sum 2");

  fx._ctrl.add_model(lib_name, "gain");

  bool flag = fx._ctrl.connect("Sum 1", 0, "gain", 0);
  EXPECT_EQ(flag, true) << "Connect should work.";

  flag = fx._ctrl.connect("gain", 0, "Sum 1", 0);
  EXPECT_EQ(flag, true) << "Connect should work.";

  flag = fx._ctrl.connect("Sum 1", 0, "Sum 2", 0);
  EXPECT_EQ(flag, true) << "Connect should work.";

  flag = fx._ctrl.connect("Sum 2", 0, "Sum 1", 1);
  EXPECT_EQ(flag, true) << "Connect should work.";

  flag = fx._ctrl.connect("Sum 2", 0, "Delay 1", 0);
  EXPECT_EQ(flag, true) << "Connect should work.";

  flag = fx._ctrl.connect("Delay 1", 0, "Sum 2", 1);
  EXPECT_EQ(flag, true) << "Connect should work.";

  check_algebraic_loops(fx, {
                                {"Sum 1", "Sum 2"},
                                {"Sum 1", "gain"},
                            });

  auto order = fx._ctrl.execution_order();
  check_execution_order(
      order,
      {}); // execution order should be empty since there is an algebraic loop
}