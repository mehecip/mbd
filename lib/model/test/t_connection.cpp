
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
enum class foo : std::uint32_t
{
};

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

  const auto [f_state, from] =
      end_point::build(&src, 2, port_dir_t::OUT); // foo
  const auto [to_state, to] =
      end_point::build(&snk, 3, port_dir_t::IN); // not foo

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

  auto [f_state, from] = end_point::build(&src, 0, port_dir_t::IN);
  auto [to_state, to] = end_point::build(&snk, 1, port_dir_t::IN);

  auto [state, conn] = connection::build(*from, *to);
  EXPECT_EQ(state, connection_state::ERR_SAME_DIRECTION);
  EXPECT_FALSE(conn);
}

TEST(ConnectionTest, SameModelSameDirectionOut)
{
  my_source src;

  auto [f_state, from] = end_point::build(&src, 0, port_dir_t::IN);
  auto [to_state, to] = end_point::build(&src, 0, port_dir_t::IN);

  const auto [state, conn] = connection::build(*from, *to);
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

  auto [f_state, from] = end_point::build(&src, 2, port_dir_t::OUT); // foo
  auto [to_state, to] = end_point::build(&snk, 2, port_dir_t::IN);   // foo

  auto [state, conn] = connection::build(*from, *to);
  EXPECT_EQ(state, connection_state::CONNECTED);
  EXPECT_TRUE(conn);

  EXPECT_FALSE(src._out_ports[0].is_connected());
  EXPECT_FALSE(src._out_ports[1].is_connected());
  EXPECT_TRUE(src._out_ports[2].is_connected());

  EXPECT_FALSE(snk._in_ports[0].is_connected());
  EXPECT_FALSE(snk._in_ports[1].is_connected());
  EXPECT_TRUE(snk._in_ports[2].is_connected());
  EXPECT_FALSE(snk._in_ports[3].is_connected());
}

TEST(ConnectionTest, DisconnectedCustomType)
{
  my_source src;
  my_sink snk;

  auto [f_state, from] = end_point::build(&src, 2, port_dir_t::OUT); // foo
  auto [to_state, to] = end_point::build(&snk, 2, port_dir_t::IN);   // foo

  {
    auto [state, conn] = connection::build(*from, *to);
    EXPECT_EQ(state, connection_state::CONNECTED);
    EXPECT_TRUE(conn);

    EXPECT_FALSE(src._out_ports[0].is_connected());
    EXPECT_FALSE(src._out_ports[1].is_connected());
    EXPECT_TRUE(src._out_ports[2].is_connected());

    EXPECT_FALSE(snk._in_ports[0].is_connected());
    EXPECT_FALSE(snk._in_ports[1].is_connected());
    EXPECT_TRUE(snk._in_ports[2].is_connected());
    EXPECT_FALSE(snk._in_ports[3].is_connected());
  }

  EXPECT_FALSE(src._out_ports[0].is_connected());
  EXPECT_FALSE(src._out_ports[1].is_connected());
  EXPECT_FALSE(src._out_ports[2].is_connected());

  EXPECT_FALSE(snk._in_ports[0].is_connected());
  EXPECT_FALSE(snk._in_ports[1].is_connected());
  EXPECT_FALSE(snk._in_ports[2].is_connected());
  EXPECT_FALSE(snk._in_ports[3].is_connected());
}

TEST(ConnectionTest, UpdateAfterSimpleConnect)
{
  my_source src;
  my_sink snk;

  auto [f_state, from] = end_point::build(&src, 0, port_dir_t::OUT);
  auto [to_state, to] = end_point::build(&snk, 0, port_dir_t::IN);

  auto [state, conn] = connection::build(*from, *to);
  EXPECT_EQ(state, connection_state::CONNECTED);
  EXPECT_TRUE(conn);

  for (std::size_t tick = 0ull; tick < 1000; ++tick)
  {
    EXPECT_NE(src._out_ports[0].read_data<float>(), tick);
    EXPECT_NE(snk.get_input<float>(0), tick);

    src.update(tick);
    EXPECT_EQ(src._out_ports[0].read_data<float>(), tick);

    // as an optimisation, the outport auto updates the inport on source
    // update() call so we get the sink input  has the data available
    // before calling update() on the sink
    EXPECT_EQ(snk.get_input<float>(0), tick);
    snk.update(tick);
    EXPECT_EQ(snk.get_input<float>(0), tick);
  }
}

TEST(ConnectionTest, UpdateAfterSimpleDisconnect)
{
  my_source src;
  my_sink snk;

  auto [f_state, from] = end_point::build(&src, 0, port_dir_t::OUT);
  auto [to_state, to] = end_point::build(&snk, 0, port_dir_t::IN);

  {
    auto [state, conn] = connection::build(*from, *to);
    EXPECT_EQ(state, connection_state::CONNECTED);
    EXPECT_TRUE(conn);

    for (std::size_t tick = 0ull; tick < 1000; ++tick)
    {
      EXPECT_NE(src._out_ports[0].read_data<float>(), tick);
      EXPECT_NE(snk.get_input<float>(0), tick);

      src.update(tick);
      EXPECT_EQ(src._out_ports[0].read_data<float>(), tick);

      // as an optimisation, the outport auto updates the inport on source
      // update() call so we get the sink input  has the data available
      // before calling update() on the sink
      EXPECT_EQ(snk.get_input<float>(0), tick);
      snk.update(tick);
      EXPECT_EQ(snk.get_input<float>(0), tick);
    }

  } // RAII disconnects on out of scope

  for (std::size_t tick = 1000ull; tick < 2000; ++tick)
  {
    src.update(tick);
    EXPECT_EQ(src._out_ports[0].read_data<float>(), tick);

    // since the connection is down, the input resets to default value
    snk.update(tick);
    EXPECT_EQ(snk.get_input<float>(0), 3.14f);
  }
}

TEST(ConnectionTest, UpdateAfterMultiConnect)
{
  my_source src;
  my_sink snk1, snk2;
  {
    auto [f_state, from] = end_point::build(&src, 0, port_dir_t::OUT);
    auto [to_snk1_state, to_snk1] = end_point::build(&snk1, 0, port_dir_t::IN);

    auto [state1, conn1] = connection::build(*from, *to_snk1);
    EXPECT_EQ(state1, connection_state::CONNECTED);
    EXPECT_TRUE(conn1);

    {
      auto [to_snk2_state, to_snk2] =
          end_point::build(&snk2, 0, port_dir_t::IN);

      auto [state2, conn2] = connection::build(*from, *to_snk2);
      EXPECT_EQ(state2, connection_state::CONNECTED);
      EXPECT_TRUE(conn2);

      for (std::size_t tick = 0ull; tick < 1000; ++tick)
      {
        EXPECT_NE(src._out_ports[0].read_data<float>(), tick);
        EXPECT_NE(snk1.get_input<float>(0), tick);
        EXPECT_NE(snk2.get_input<float>(0), tick);

        src.update(tick);
        EXPECT_EQ(src._out_ports[0].read_data<float>(), tick);

        // as an optimisation, the outport auto updates the inport on source
        // update() call so we get the sink input  has the data available
        // before calling update() on the sink
        EXPECT_EQ(snk1.get_input<float>(0), tick);
        snk1.update(tick);
        EXPECT_EQ(snk1.get_input<float>(0), tick);

        EXPECT_EQ(snk2.get_input<float>(0), tick);
        snk2.update(tick);
        EXPECT_EQ(snk2.get_input<float>(0), tick);
      }
    } // RAII disconnects src -> snk2

    for (std::size_t tick = 1000ull; tick < 2000; ++tick)
    {
      src.update(tick);
      EXPECT_EQ(src._out_ports[0].read_data<float>(), tick);

      EXPECT_EQ(snk1.get_input<float>(0), tick);
      snk1.update(tick);
      EXPECT_EQ(snk1.get_input<float>(0), tick);

      // since the connection is down, the input resets to default value
      EXPECT_EQ(snk2.get_input<float>(0), 3.14f);
      snk2.update(tick);
      EXPECT_EQ(snk2.get_input<float>(0), 3.14f);
    }
  } // RAII disconnects src -> snk1

  for (std::size_t tick = 1000ull; tick < 2000; ++tick)
  {
    src.update(tick);
    EXPECT_EQ(src._out_ports[0].read_data<float>(), tick);

    // since the connection is down, the input resets to default value
    EXPECT_EQ(snk1.get_input<float>(0), 3.14f);
    snk1.update(tick);
    EXPECT_EQ(snk1.get_input<float>(0), 3.14f);

    // since the connection is down, the input resets to default value
    EXPECT_EQ(snk2.get_input<float>(0), 3.14f);
    snk2.update(tick);
    EXPECT_EQ(snk2.get_input<float>(0), 3.14f);
  }
}