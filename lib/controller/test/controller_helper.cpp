#include "controller_helper.hpp"

std::unordered_map<mbd::log_level, std::vector<std::string>>
    ControllerHelper::_log;

ControllerHelper::ControllerHelper() : _ctrl(ControllerHelper::message_callback)
{}

void ControllerHelper::addAllModels(const mbd::lib &lib)
{
  _ctrl.add_library(lib);
  for (const auto &type : lib.get_model_types())
  {
    _ctrl.add_model(lib.get_name(), type);
  }
}

void ControllerHelper::addModels(const mbd::lib &lib,
                                 const std::vector<std::string> &models)
{
  _ctrl.add_library(lib);
  for (const auto &type : models)
  {
    _ctrl.add_model(lib.get_name(), type);
  }
}

ControllerHelper::~ControllerHelper()
{
  // Clear the log after each test to avoid interference
  _log.clear();
}

void ControllerHelper::message_callback(log_level lvl, const std::string &msg)
{
  std::cout << level_info(lvl) << ": " << msg << "\n";
  _log[lvl].push_back(msg);
}
