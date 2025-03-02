
#include "gtest/gtest.h"
#include <csignal>
#include <cstddef>
#include <cstdint>
#include <gtest/gtest.h>
#include <memory>
#include <signal.h>
#include <optional>
#include <string>

#include "library.hpp"
#include "model.hpp"

using namespace mbd;

namespace
{
class my_source : public model
{
public:
  my_source() : model("my_source")
  {
    add_input("in", 0.01);
    add_output("out1", 42.0f);
    add_param("my param", -2);
  }

  void update(const std::size_t tick) override
  {
    set_output<float>("out1", (float)tick);
  }

  bool is_source() const override { return true; }
};

class my_sink : public model
{
public:
  my_sink() : model("my_source")
  {
    add_input("in1", 3.14f);
  }

  void update(const std::size_t tick) override {}
  bool is_source() const override { return false; }
};
} // namespace

TEST(LibraryTest, LibraryAPI)
{
  mbd::lib my_lib("my_lib");
  EXPECT_EQ(my_lib.get_name(), "my_lib");
  EXPECT_EQ(my_lib.get_model_types().size(), 0);
  EXPECT_EQ(my_lib.build_model("sink"), nullptr);
  EXPECT_EQ(my_lib.build_model("source"), nullptr);

  my_lib.register_model<my_source>("my_source");
  EXPECT_EQ(my_lib.get_model_types().size(), 1);
  EXPECT_EQ(my_lib.build_model("source"), nullptr);

  const auto lib_mdl = my_lib.build_model("my_source");
  EXPECT_NE(lib_mdl, nullptr);
  EXPECT_TRUE(lib_mdl->is_source());

  my_lib.register_model<my_sink>("my_sink");
  EXPECT_EQ(my_lib.get_model_types().size(), 2);

   const auto lib_snk = my_lib.build_model("my_sink");
  EXPECT_NE(lib_snk, nullptr);
  EXPECT_NE(lib_snk, lib_mdl);
  EXPECT_FALSE(lib_snk->is_source());

  my_lib.register_model<my_sink>("my_sink");
  EXPECT_EQ(my_lib.get_model_types().size(), 2);

}
