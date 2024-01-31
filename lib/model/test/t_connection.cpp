
#include "gtest/gtest.h"
#include <cstdint>
#include <gtest/gtest.h>
#include <optional>
#include <string>

#include "connection.hpp"
#include "connection_state.hpp"
#include "model.hpp"

using namespace mbd;

namespace
{
enum class foo : std::uint32_t {};

class my_source : public model
{
public:
  my_source() : model("my_source")
  {
    add_input("in", 3.14f);
    add_output("out1", 42.0f);
    add_output<std::string>("out2", "string");
    add_output<foo>("foo", {});
  }

  void update(const std::size_t tick) override
  {
    set_output<float>("out1", tick);
  }

  bool is_source() const override { return true; }
};

class my_sink : public model
{
public:
  my_sink() : model("my_source")
  {
    add_input("in1", 3.14f);
    add_input<std::string>("in2", "string");
    add_input<foo>("foo", {});
    add_input<std::uint32_t>("not foo", {});
    add_output<std::string>("out2", "string");
  }

  void update(const std::size_t tick) override {}
  bool is_source() const override { return false; }
};
} // namespace


TEST(ConnectionTest, SameModel)
{
  my_source src;

  const end_point from{&src, 0, port_dir_t::OUT};
  const end_point to{&src, 0, port_dir_t::IN};

  const auto [state, conn] = connection::build(from, to);
  EXPECT_EQ(state, connection_state::ERR_SAME_MODEL);
  EXPECT_FALSE(conn);
}


TEST(ConnectionTest, DifferentType)
{
  my_source src;
  my_sink snk;

  const auto [f_state, from] = end_point::build(&src, 0, port_dir_t::OUT);
  const auto [to_state, to] = end_point::build(&snk, 1, port_dir_t::IN);

  auto [state, conn] = connection::build(*from, *to);
  EXPECT_EQ(state, connection_state::ERR_DIFF_TYPE);
  EXPECT_FALSE(conn);
}


TEST(ConnectionTest, DifferentCustomType)
{
  my_source src;
  my_sink snk;

  const auto [f_state, from] = end_point::build(&src, 2, port_dir_t::OUT); // foo
  const auto [to_state, to] = end_point::build(&snk, 3, port_dir_t::IN); // not foo

  auto [state, conn] = connection::build(*from, *to);
  EXPECT_EQ(state, connection_state::ERR_DIFF_TYPE);
  EXPECT_FALSE(conn);
}


TEST(ConnectionTest, InvalidPortIdx)
{
  my_source src;
  my_sink snk;

  const auto [f_state, from] = end_point::build(&src, 3, port_dir_t::OUT);
  EXPECT_EQ(from, std::nullopt);
  EXPECT_EQ(f_state, connection_state::ERR_OUT_PORT_INVALID);

  const auto [to_state, to] = end_point::build(&snk, 4, port_dir_t::IN);
  EXPECT_EQ(to, std::nullopt);
  EXPECT_EQ(to_state, connection_state::ERR_IN_PORT_INVALID);
}


TEST(ConnectionTest, SameDirectionOut)
{
  my_source src;
  my_sink snk;

  const auto [f_state, from] = end_point::build(&src, 0, port_dir_t::OUT);
  const auto [to_state, to] = end_point::build(&snk, 0, port_dir_t::OUT);

  auto [state, conn] = connection::build(*from, *to);
  EXPECT_EQ(state, connection_state::ERR_SAME_DIRECTION);
  EXPECT_FALSE(conn);
}


TEST(ConnectionTest, SameDirectionIn)
{
  my_source src;
  my_sink snk;

  const auto [f_state, from] = end_point::build(&src, 0, port_dir_t::IN);
  const auto [to_state, to] = end_point::build(&snk, 1, port_dir_t::IN);

  auto [state, conn] = connection::build(*from, *to);
  EXPECT_EQ(state, connection_state::ERR_SAME_DIRECTION);
  EXPECT_FALSE(conn);
}


TEST(ConnectionTest, SameModelSameDirectionOut)
{
  my_source src;

  const end_point from{&src, 0, port_dir_t::OUT};
  const end_point to{&src, 0, port_dir_t::OUT};

  const auto [state, conn] = connection::build(from, to);
  EXPECT_EQ(state, connection_state::ERR_SAME_MODEL);
  EXPECT_FALSE(conn);
}


TEST(ConnectionTest, SameModelSameDirectionIn)
{
  my_source src;

  const end_point from{&src, 0, port_dir_t::IN};
  const end_point to{&src, 0, port_dir_t::IN};

  const auto [state, conn] = connection::build(from, to);
  EXPECT_EQ(state, connection_state::ERR_SAME_MODEL);
  EXPECT_FALSE(conn);
}


TEST(ConnectionTest, ConnectedCustomType)
{
  my_source src;
  my_sink snk;

  const auto [f_state, from] = end_point::build(&src, 2, port_dir_t::OUT); // foo
  const auto [to_state, to] = end_point::build(&snk, 2, port_dir_t::IN); // foo

  auto [state, conn] = connection::build(*from, *to);
  EXPECT_EQ(state, connection_state::CONNECTED);
  EXPECT_TRUE(conn);
}