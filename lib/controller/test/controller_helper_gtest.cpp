#include "controller_helper.hpp"
#include "gtest/gtest.h"

void check_execution_order(std::vector<mbd::model_vec_t> &actual_models,
                           std::vector<std::vector<std::string>> expected_names)
{
  ASSERT_EQ(actual_models.size(), expected_names.size())
      << "Execution order size mismatch.";

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

  // Sort the expected names for comparison
  std::sort(actual_names.begin(), actual_names.end());
  std::sort(expected_names.begin(), expected_names.end(),
            [](const auto &a, const auto &b) { return a < b; });

  EXPECT_EQ(actual_names, expected_names);
}

void check_algebraic_loops(ControllerHelper &fx,
                           std::vector<std::vector<std::string>> expected_names)
{
  if (expected_names.empty())
  {
    ASSERT_FALSE(fx._ctrl.has_algebraic_loops())
        << "There should be no algebraic loops.";
    return;
  }
  else
  {
    ASSERT_TRUE(fx._ctrl.has_algebraic_loops())
        << "There should be algebraic loops.";
  }

  const auto &actual_loops = fx._ctrl.get_algebraic_loops();

  ASSERT_EQ(actual_loops.size(), expected_names.size())
      << "Algebraic loops size mismatch.";

  std::vector<std::vector<std::string>> actual_names;
  for (const auto &loop : actual_loops)
  {
    std::vector<std::string> names;
    for (const auto &model : loop)
    {
      names.push_back(model->get_name());
    }
    std::sort(names.begin(), names.end());
    actual_names.push_back(names);
  }

  // Sort the expected names for comparison
  std::sort(actual_names.begin(), actual_names.end());
  std::sort(expected_names.begin(), expected_names.end(),
            [](const auto &a, const auto &b) { return a < b; });

  EXPECT_EQ(actual_names, expected_names);
}