#include <cstdint>
#include <gtest/gtest.h>
#include <string>

#include "node.hpp"
#include "uuid.hpp"

TEST(NodeTest, Uuid)
{
  mbd::uuid uuid("custom uuid");

  mbd::node n("my_node", uuid);
  EXPECT_EQ(n._uuid, uuid);
  EXPECT_EQ(n._name, "my_node");

  mbd::node on("my_other_node");
  EXPECT_NE(on._uuid, uuid);
  EXPECT_EQ(on._name, "my_other_node");

  EXPECT_NE(on._uuid, n._uuid);
}

TEST(NodeTest, Inputs)
{
  mbd::node n("my_node");

  n.add_input<std::string>("in1", "My Init Data");
  n.add_input<bool>("in2", true);

  /* Inputs */
  EXPECT_EQ(n._in_ports.size(), 2);
  EXPECT_EQ(n._in_ports_map.size(), 2);

  EXPECT_NE(n._in_ports_map.find("in1"), n._in_ports_map.end());
  EXPECT_EQ(n.get_input<std::string>("in1"), "My Init Data");
  EXPECT_EQ(n.get_input<std::string>(0), "My Init Data");

  EXPECT_NE(n._in_ports_map.find("in2"), n._in_ports_map.end());
  EXPECT_EQ(n.get_input<bool>("in2"), true);
  EXPECT_EQ(n.get_input<bool>(1), true);
}

TEST(NodeTest, Outputs)
{
  mbd::node n("my_node");
  n.add_output("out", 32.4f);

  /* Outputs */
  EXPECT_EQ(n._out_ports.size(), 1);
  EXPECT_NE(n._out_ports_map.find("out"), n._out_ports_map.end());
  EXPECT_EQ(n._out_ports[0].read_data<float>(), 32.4f);

  EXPECT_NO_THROW(n.set_output(0, 0.01f));
  EXPECT_EQ(n._out_ports[0].read_data<float>(), 0.01f);

  EXPECT_NO_THROW(n.set_output("out", -11.01f));
  EXPECT_EQ(n._out_ports[0].read_data<float>(), -11.01f);
}

TEST(NodeTest, Params)
{
  mbd::node n("my_node");
  n.add_param("param", -1);

  /* Params */
  EXPECT_EQ(n._params.size(), 1);
  EXPECT_EQ(n._params_map.size(), 1);
  EXPECT_NE(n._params_map.find("param"), n._params_map.end());

  EXPECT_EQ(n.get_param<int>("param"), -1);
  EXPECT_EQ(n.get_param<int>(0), -1);

  EXPECT_NO_THROW(n.set_param(0, 42));

  EXPECT_EQ(n._params.size(), 1);
  EXPECT_EQ(n._params_map.size(), 1);
  EXPECT_NE(n._params_map.find("param"), n._params_map.end());

  EXPECT_EQ(n.get_param<int>("param"), 42);
  EXPECT_EQ(n.get_param<int>(0), 42);
}

TEST(NodeTest, Custom_type)
{

  struct my_base_type
  {
    bool flag = true;
  };

  struct my_type : my_base_type
  {
    std::int32_t value = -11;

    bool operator==(const my_type &rhs) const
    {
      return value == rhs.value && flag == rhs.flag;
    }
  };

  mbd::node n("my_node");

  n.add_input<my_type>("in1", {});
  n.add_param("param", my_type{{false}, 42});

  my_type in1;
  EXPECT_EQ(n.get_input<my_type>("in1"), in1);

  my_type param{{false}, 42};
  EXPECT_EQ(n.get_param<my_type>("param"), param);
}