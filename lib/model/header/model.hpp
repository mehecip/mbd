#pragma once
#include <cstdint>
#include <memory>
#include <string>

#include "connection_state.hpp"
#include "log_level.hpp"
#include "msg_dispatcher.hpp"
#include "node.hpp"

namespace mbd
{

class model : public msg_dispatcher, public node
{
public:
  using ptr_t = std::unique_ptr<model>;

public:
  // must be used to add inpus/outputs to your model
  model(const std::string &n);

  model(const model&) = delete;
  model(const model&&) = delete;

  virtual ~model(){};

  // called when executing
  virtual void update(std::uint64_t tick) = 0;

  // a model is feedthrough if the CURRENT value of an input determines the
  // CURRENT value of an output e.g.: sum -> true source -> false unit delay ->
  // false (the CURRENT value of the input determines the NEXT value of the
  // output) this method is used by a controller to determine the execution
  // order of each model
  virtual bool is_feedthrough() const = 0;

  const std::string &get_name() const;
  void set_name(const std::string& name);
};
} // namespace mbd
