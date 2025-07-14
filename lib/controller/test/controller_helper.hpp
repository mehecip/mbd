#pragma once

#include "controller.hpp"
#include "library_fixtures/math_lib.hpp"
#include "model.hpp"

using namespace mbd;
using namespace tst;

class ControllerHelper
{

public:
  ControllerHelper();
  void addAllModels(const mbd::lib &lib);
  void addModels(const mbd::lib &lib, const std::vector<std::string> &models);

  ~ControllerHelper();

  mbd::controller _ctrl;
  static std::unordered_map<mbd::log_level, std::vector<std::string>> _log;

private:
  static void message_callback(log_level lvl, const std::string &msg);
};



void check_execution_order(std::vector<mbd::model_vec_t> &actual_models,
                            std::vector<std::vector<std::string>> expected_names);
void check_algebraic_loops(ControllerHelper &fx,
                            std::vector<std::vector<std::string>> expected_names);