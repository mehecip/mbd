#include "controller.hpp"
#include "log_level.hpp"
#include "math_lib.hpp"
#include "model.hpp"

#include "gain.hpp"
#include "mathop.hpp"
#include "sink.hpp"
#include "source.hpp"
#include "type_convertor.hpp"

#include <iostream>

using namespace mbd;
using namespace mbd::impl;

void message_callback(log_level lvl, const std::string &msg)
{
  std::cout << level_info(lvl) << ": " << msg << "\n";
}

void controller_example()
{
  std::cout << "\n\n"
            << "\t\t Controller Example"
            << "\n\n";

  const auto math_lib = get_math_lib<double>("Double");
  const auto lib_name = math_lib.get_name();

  mbd::controller cntrl(message_callback);
  cntrl.add_library(math_lib);

  // add models from the library
  for (const auto &type : math_lib.get_model_types())
  {
    cntrl.add_model(lib_name, type);
  }

  // connect the models
  cntrl.connect("constant source", 0, "add", 0);
  cntrl.connect("linear source", 0, "type convertor", 0);
  cntrl.connect("type convertor", 0, "add", 1);
  cntrl.connect("add", 0, "unit delay", 0);
  cntrl.connect("unit delay", 0, "gain", 0);
  cntrl.connect("gain", 0, "sink", 0);

  // set parameters
  auto csrc = cntrl.get<mbd::impl::const_source<double>>("constant source");
  csrc->set_value(1'000.0);
  csrc->set_init_val(-100.0);
  csrc->set_step_tick(1'001);

  auto lsrc = cntrl.get<mbd::impl::linear_source<double>>("linear source");
  lsrc->set_param(3.1415926f, 0.001f);

  auto gain = cntrl.get<mbd::impl::gain<double>>("gain");
  gain->set_value(2.0);

  std::uint64_t ticks = 15'601;
  cntrl.run(ticks);

  auto sink_ = cntrl.get<mbd::impl::sink<double>>("sink");
  std::cout << "Sink Value after "<< ticks << " ticks: " << sink_->read() << "\n\n";
}

int main()
{
  std::cout << "The Design: \n\n";

  std::cout << "|Constant Source| -------------------------------> |     | \n";
  std::cout << "                                                   | Sum | -------> | Gain | -------> | Unit Delay | -------> | Sink | \n";
  std::cout << "|Linear Source| -------> |Type Convertor| -------> |     | \n";

  std::cout << "\n\n\n\n";


  controller_example();

  return 0;
}