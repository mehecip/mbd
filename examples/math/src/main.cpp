#include "log_level.hpp"
#include "math_lib.hpp"
#include "model.hpp"

#include "gain.hpp"
#include "mathop.hpp"
#include "sink.hpp"
#include "source.hpp"
#include "type_convertor.hpp"

#include <iostream>

#include <chrono>

#include <assert.h>

using namespace mbd;
using namespace mbd::impl;

class Timer
{
public:
  Timer() : beg_(clock_::now()) {}
  void reset() { beg_ = clock_::now(); }

  double elapsed_sec() const
  {
    return std::chrono::duration_cast<second_>(clock_::now() - beg_).count();
  }

  double elapsed_microsec() const
  {
    return std::chrono::duration_cast<microsec_>(clock_::now() - beg_).count();
  }

  double elapsed_mili() const
  {
    return std::chrono::duration_cast<milisec_>(clock_::now() - beg_).count();
  }

private:
  typedef std::chrono::high_resolution_clock clock_;
  typedef std::chrono::duration<double, std::ratio<1>> second_;
  typedef std::chrono::duration<double, std::ratio<1, 1'000>> milisec_;
  typedef std::chrono::duration<double, std::ratio<1, 1'000'000>> microsec_;

  std::chrono::time_point<clock_> beg_;
};

void message_callback(log_level lvl, const std::string &msg)
{
  std::cout << level_info(lvl) << ": " << msg << "\n";
}

#if BUILD_CONTROLLER_EXAMPLES

#include "controller.hpp"
void controller_example()
{

  std::cout << "\n\n"
            << "\t\t Controller Example"
            << "\n\n";

  Timer t;

  const auto math_lib = get_math_lib<double>("Double");
  const auto lib_name = math_lib.get_name();

  mbd::controller cntrl(message_callback);
  cntrl.add_library(math_lib);

  bool flag = true;
  for (const auto &type : math_lib.get_model_types())
  {
    const auto mdl = cntrl.add_model(lib_name, type);
    flag &= mdl != nullptr;
  }

  assert(flag == true && "Add model should work.");

  std::cout << "Model creation and registration duration: " << t.elapsed_microsec()
            << " microseconds." << std::endl;

  t.reset();

  flag &= cntrl.connect("constant source", 0, "add", 0);
  flag &= cntrl.connect("linear source", 0, "type convertor", 0);
  flag &= cntrl.connect("type convertor", 0, "add", 1);
  flag &= cntrl.connect("add", 0, "unit delay", 0);
  flag &= cntrl.connect("unit delay", 0, "gain", 0);
  flag &= cntrl.connect("gain", 0, "sink", 0);

  assert(flag == true && "Connect should work.");

  std::cout << "Connecting duration: " << t.elapsed_microsec()
            << " microseconds. " << std::endl;

  auto csrc = cntrl.get<mbd::impl::const_source<double>>("constant source");
  csrc->set_value(10'000.0);
  csrc->set_init_val(-100.0);
  csrc->set_step_tick(10'001);

  auto lsrc = cntrl.get<mbd::impl::linear_source<double>>("linear source");
  lsrc->set_param(-3.1415926f, 0.001f);

  auto gain = cntrl.get<mbd::impl::gain<double>>("gain");
  gain->set_value(2.0);

  t.reset();

  std::uint64_t ticks = 10'000'000;
  cntrl.run(ticks);

  std::cout << ticks << " ticks executed in " << t.elapsed_sec()
            << " seconds. Average tick duration "
            << t.elapsed_microsec() / ticks << " microseconds." << std::endl;

  auto sink_ = cntrl.get<mbd::impl::sink<double>>("sink");
  std::cout << "Sink Value after execution: " << sink_->read() << "\n\n";

  flag &= cntrl.disconnect("constant source", 0, "add", 0);

  assert(flag == true && "Disconnect should work");
}

void algebraic_loop()
{
  std::cout << "\n\n \t\t Algebraic Loops Example \n\n";

  const auto math_lib = get_math_lib<std::uint64_t>("UINT 64");
  const auto lib_name = math_lib.get_name();

  {
    mbd::controller cntrl(message_callback);
    cntrl.add_library(math_lib);

    auto delay1 = cntrl.add_model(lib_name, "delay");
    delay1->set_name("Delay 1");

    auto delay2 = cntrl.add_model(lib_name, "delay");
    delay2->set_name("Delay 2");

    auto sum1 = cntrl.add_model(lib_name, "sum");
    sum1->set_name("Sum 1");

    auto sum2 = cntrl.add_model(lib_name, "sum");
    sum2->set_name("Sum 2");

    bool flag = true;

    flag &= cntrl.connect("Delay 1", 0, "Sum 1", 0);
    flag &= cntrl.connect("Sum 1", 0, "Delay 1", 0);
    flag &= cntrl.connect("Sum 1", 0, "Sum 2", 0);
    flag &= cntrl.connect("Delay 2", 0, "Sum 2", 1);
    flag &= cntrl.connect("Sum 2", 0, "Sum 1", 1);
    flag &= cntrl.connect("Sum 2", 0, "Delay 2", 0);

    assert(flag && "All connections should have been made");

    std::cout << "LOOP 1 *********** [Sum 1] <-> [Sum 2] ********* \n ";
    auto n = cntrl.find_algebraic_loops();

    assert(n == 1 && "There should one algebraic loop: [Sum 1] <-> [Sum 2]");
  }

  {
    mbd::controller cntrl(message_callback);
    cntrl.add_library(math_lib);

    auto delay1 = cntrl.add_model(lib_name, "delay");
    delay1->set_name("Delay 1");

    auto sum1 = cntrl.add_model(lib_name, "sum");
    sum1->set_name("Sum 1");

    auto sum2 = cntrl.add_model(lib_name, "sum");
    sum2->set_name("Sum 2");

    cntrl.add_model(lib_name, "gain");

    bool flag = true;

    flag &= cntrl.connect("Sum 1", 0, "gain", 0);
    flag &= cntrl.connect("gain", 0, "Sum 1", 0);
    flag &= cntrl.connect("Sum 1", 0, "Sum 2", 0);
    flag &= cntrl.connect("Sum 2", 0, "Sum 1", 1);
    flag &= cntrl.connect("Sum 2", 0, "Delay 1", 0);
    flag &= cntrl.connect("Delay 1", 0, "Sum 2", 1);

    assert(flag && "All connections should have been made");

    std::cout << "LOOP 2 *********** [Sum 1] <-> [gain] and [Sum 1] <-> [Sum 2] ********* \n ";
    auto n = cntrl.find_algebraic_loops();

    assert(n == 2 && "There should two algebraic loops: [Sum 1] <-> [gain] and [Sum 1] <-> [Sum 2]");
  }
}

#endif

void example()
{

  std::cout << "\n\n \t\t Example w/o controller \n\n";

  /****************** BUILD ******************/

  mbd::lib math_lib("Math Lib");

  std::uint64_t csrc_step_tick = 1000ull;
  double csrc_val = 1'000.0;
  double csrc_init_val = 0.0;
  math_lib.register_model<const_source<double>>("Constant Source", csrc_val,
                                                csrc_init_val, csrc_step_tick);

  float lsrc_init_val = -110.0f;
  float lsrc_step_val = 1.0f;
  math_lib.register_model<linear_source<float>>("Linear Source", lsrc_init_val,
                                                lsrc_step_val);

  double gain_val = 2.0;
  math_lib.register_model<gain<double>>("Gain", 2.0);

  math_lib.register_model<type_convertor<float, double>>("Type Convertor");
  math_lib.register_model<add<double>>("Sum");
  math_lib.register_model<sink<double>>("Sink");

  auto csrc = math_lib.build_model("Constant Source");
  auto lsrc = math_lib.build_model("Linear Source");
  auto type_conv = math_lib.build_model("Type Convertor");
  auto sum = math_lib.build_model("Sum");
  auto gain_ = math_lib.build_model("Gain");
  auto sink_ = math_lib.build_model("Sink");

  /****************** CONNECT ******************/
  end_point csrc_0{csrc.get(), 0, port_dir_t::OUT};
  end_point sum_0{sum.get(), 0, port_dir_t::IN};

  auto [s1, csrc_sum] = connection::build(csrc_0, sum_0);

  end_point lsrc_0{lsrc.get(), 0, port_dir_t::OUT};
  end_point type_conv_0{type_conv.get(), 0, port_dir_t::IN};

  auto [s2, lsrc_type_conv] = connection::build(lsrc_0, type_conv_0);

  end_point type_conv_o_0{type_conv.get(), 0, port_dir_t::OUT};
  end_point sum_1{sum.get(), 1, port_dir_t::IN};

  auto [s3, type_conv_sum] = connection::build(type_conv_o_0, sum_1);

  end_point sum_o_0{sum.get(), 0, port_dir_t::OUT};
  end_point gain_0{gain_.get(), 0, port_dir_t::IN};

  auto [s4, sum_gain] = connection::build(sum_o_0, gain_0);

  end_point gain_o_0{gain_.get(), 0, port_dir_t::OUT};
  end_point sync_0{sink_.get(), 0, port_dir_t::IN};

  auto [s5, gain_sync] = connection::build(gain_o_0, sync_0);

  /****************** EXECUTE ******************/
  // execution order is:
  // 0 -> Constant Source and Linear Source
  // 1 -> Type Convertor
  // 2 -> Sum
  // 3 -> Gain
  // 4 -> Sink

  std::uint64_t ticks = 1'000'000ull;

  for (std::uint64_t i = 0; i < ticks; i++)
  {
    csrc->update(i);
    lsrc->update(i);

    type_conv->update(i);

    sum->update(i);

    gain_->update(i);

    sink_->update(i);
  }

  /****************** CHECK ******************/

  // (ticks - 1) because on the first tick linear_source sets the init value as
  // output and then updates the value for the next tick
  double expected_val =
      (csrc_val + (((double)(ticks - 1) * lsrc_step_val) + lsrc_init_val)) *
      gain_val;

  double actual_val = static_cast<sink<double> *>(sink_.get())->read();

  std::cout << "\n\n"
            << "Expected sink value: " << expected_val << ". "
            << "Actual value: " << actual_val << ".\n\n";
}

int main()
{
  std::cout << "The Design: \n\n";
  std::cout << "|Constant Source| -------------------------------> |     | \n";
  std::cout << "                                                   | Sum | "
            << "-------> | Gain | -------> | Unit Delay | -------> | Sink | \n";
  std::cout << "|Linear Source| -------> |Type Convertor| -------> |     | \n";
  std::cout << "\n\n\n\n";

  example();

#if BUILD_CONTROLLER_EXAMPLES
  controller_example();

  algebraic_loop();
#endif

  return 0;
}