
#include <benchmark/benchmark.h>

#include <vector>

#include "controller.hpp"
#include "controller_helper.hpp"
#include "library_fixtures/math_lib.hpp"
#include "model.hpp"

static void tPerfTest(benchmark::State &state)
{

  auto lib = get_math_lib<double>("double");

  ControllerHelper fx;

  fx.addModels(lib, {"const_src", "gain", "sink", "lin_src", "double_to", "sum",
                     "delay"});

  auto flag = fx._ctrl.connect("constant source", 0, "add", 0);
  flag &= fx._ctrl.connect("linear source", 0, "type convertor", 0);
  flag &= fx._ctrl.connect("type convertor", 0, "add", 1);
  flag &= fx._ctrl.connect("add", 0, "unit delay", 0);
  flag &= fx._ctrl.connect("unit delay", 0, "gain", 0);
  flag &= fx._ctrl.connect("gain", 0, "sink", 0);

  assert(flag == true && "Connect should work.");

  auto csrc = fx._ctrl.get<const_source<double>>("constant source");
  csrc->set_value(10'000.0);
  csrc->set_init_val(-100.0);
  csrc->set_step_tick(10'001);

  auto lsrc = fx._ctrl.get<linear_source<double>>("linear source");
  lsrc->set_param(-3.1415926f, 0.001f);

  auto g = fx._ctrl.get<gain<double>>("gain");
  g->set_value(2.0);

  // do not store data in the sink
  auto s = fx._ctrl.get<sink<double>>("sink");
  s->store_data(false);

  for (auto _ : state)
  {
    // assuming a tick rate of 0.1 seconds
    // and a simulation time of pi months
    constexpr static std::uint64_t ticks = 3.1415926 * 31 * 24 * 60 * 60 * 10;
    fx._ctrl.run(ticks);
  }
};

BENCHMARK(tPerfTest)
    ->Unit(benchmark::kMillisecond)
    ->Iterations(1)
    ->UseRealTime();