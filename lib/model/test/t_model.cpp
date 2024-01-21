#include <cstddef>
#include <cstdint>
#include <gtest/gtest.h>
#include <string>

#include "log_level.hpp"
#include "model.hpp"
#include "node.hpp"

namespace
{

class my_source : public mbd::model
{
public:
  my_source() : mbd::model("my_source")
  {
    add_output("out1", 42.0f);
    add_output("out2", true);
    add_param("param", 3.0f);
  }

  void update(const std::size_t tick) override
  {
    const auto &p = get_param<float>("param");
    set_output<float>("out1", tick * p);

    if (tick < 100)
    {
      add_message(mbd::log_level::INFO, "");
    }
    else if (tick < 200)
    {
      add_message(mbd::log_level::ERROR, "");
    }
    else if (tick < 300)
    {
      add_message(mbd::log_level::WARNING, "");
    }
    else
    {
      add_message(mbd::log_level::DEBUG, "");
    }
  }

  bool is_source() const override { return true; }
};

class my_sink : public mbd::model
{
public:
  my_sink() : mbd::model("my_source")
  {
    add_input("in", 3.14f);
    add_param("history", std::vector<float>{});
  }
  void update(const std::size_t tick) override
  {

    const auto &v = get_input<float>("in");
    auto h = get_param<std::vector<float>>("history");
    h.push_back(v);

    set_param("history", h);
  }
  bool is_source() const override { return false; }
};
} // namespace

TEST(ModelTest, ModelName)
{
  my_source src;
  my_sink sink;

  EXPECT_EQ(src.is_source(), true);
  EXPECT_EQ(sink.is_source(), false);

  EXPECT_EQ(src.get_name(), "my_source");
  EXPECT_NO_THROW(src.set_name("my_new_source"));
  EXPECT_EQ(src.get_name(), "my_new_source");
}

TEST(ModelTest, Inputs)
{
  my_source src;
  my_sink sink;

  /* Source Inputs */
  EXPECT_EQ(src._in_ports.size(), 0);
  EXPECT_EQ(src._in_ports_map.size(), 0);

  /* Sink Inputs */
  EXPECT_EQ(sink._in_ports.size(), 1);
  EXPECT_EQ(sink._in_ports_map.size(), 1);

  EXPECT_NE(sink._in_ports_map.find("in"), sink._in_ports_map.end());
  EXPECT_EQ(sink.get_input<float>("in"), 3.14f);
  EXPECT_EQ(sink.get_input<float>(0), 3.14f);

  for (std::size_t tick = 0; tick < 1'000ull; ++tick)
  {
    EXPECT_NO_THROW(src.update(tick));
    EXPECT_NO_THROW(sink.update(tick));
  }

  // nothing changes for the inputs, since they are not connected
  EXPECT_EQ(sink.get_input<float>("in"), 3.14f);
}

TEST(ModelTest, Outputs)
{
  my_source src;
  my_sink sink;

  /* Src Outputs */
  EXPECT_EQ(src._out_ports.size(), 2);
  EXPECT_NE(src._out_ports_map.find("out1"), src._out_ports_map.end());
  EXPECT_EQ(src._out_ports[0].read_data<float>(), 42.f);

  EXPECT_NE(src._out_ports_map.find("out2"), src._out_ports_map.end());
  EXPECT_EQ(src._out_ports[1].read_data<bool>(), true);

  /* Src Outputs */
  EXPECT_EQ(sink._out_ports.size(), 0);

  std::size_t ticks = 1'000;
  for (std::size_t tick = 0; tick < ticks; ++tick)
  {
    EXPECT_NO_THROW(src.update(tick));
    EXPECT_NO_THROW(sink.update(tick));
  }

  // nothing changes for the inputs/outputs, since they are not connected
  EXPECT_EQ(sink.get_input<float>("in"), 3.14f);
  const float p = src.get_param<float>(0);
  EXPECT_EQ(src._out_ports[0].read_data<float>(), (ticks - 1) * p);
}

TEST(ModelTest, Params)
{
  my_source src;
  my_sink sink;

  /* Sink Param */
  EXPECT_EQ(sink._params.size(), 1);
  EXPECT_EQ(sink._params_map.size(), 1);
  EXPECT_NE(sink._params_map.find("history"), sink._params_map.end());

  EXPECT_EQ(sink.get_param<std::vector<float>>("history"),
            std::vector<float>{});

  for (std::size_t tick = 0; tick < 1'000ull; ++tick)
  {
    EXPECT_NO_THROW(src.update(tick));
    EXPECT_NO_THROW(sink.update(tick));
  }

  const auto h = sink.get_param<std::vector<float>>("history");
  EXPECT_EQ(h.size(), 1'000);
  EXPECT_EQ(h, std::vector<float>(1'000, 3.14f));
}

TEST(ModelTest, Custom_type)
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

  my_source src;
  my_sink sink;

  src.add_input<my_type>("in1", {});
  src.add_param("custom param", my_type{false, 42});

  sink.add_input<my_type>("in2", {});
  sink.add_param("custom param", my_type{false, 42});

  my_type in1;
  EXPECT_EQ(src.get_input<my_type>("in1"), in1);
  EXPECT_EQ(sink.get_input<my_type>("in2"), in1);

  my_type param{false, 42};
  EXPECT_EQ(src.get_param<my_type>("custom param"), param);
  EXPECT_EQ(sink.get_param<my_type>("custom param"), param);
}

TEST(ModelTest, MessageDispatcher)
{
  std::size_t counter = 0ull;
  std::string log_string = "";

  const auto &msg_callback = [&counter, &log_string](const mbd::log_level level,
                                        const auto &msg) { 
                                          ++counter; 
                                          log_string = msg;};

  my_source src;
  src.add_msg_callback(msg_callback);
  
  my_sink sink;
  sink.add_msg_callback(msg_callback);

  std::size_t ticks = 1'000;
  for (std::size_t tick = 0; tick < ticks; ++tick)
  {
    EXPECT_NO_THROW(src.update(tick)); // adds messages
    EXPECT_NO_THROW(sink.update(tick)); // does not add any messages
    EXPECT_EQ(counter, tick + 1);
    EXPECT_EQ(log_string, "");
  }

  src.add_message(mbd::log_level::INFO, "my src message");
  EXPECT_EQ(counter, ticks + 1);
  EXPECT_EQ(log_string, "my src message");

  sink.add_message(mbd::log_level::INFO, "my sink message");
  EXPECT_EQ(counter, ticks + 2);
  EXPECT_EQ(log_string, "my sink message");
}