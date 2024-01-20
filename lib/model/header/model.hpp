#pragma once
#include <cstdint>
#include <memory>
#include <string>

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

  // a model is a source if the CURRENT value of an input does not determine the
  // CURRENT value of an outpu.
  // e.g.: sum -> false 
  // source -> true 
  // unit delay -> true (the CURRENT value of the input determines the NEXT value of the
  // output) 
  // this method is used by a controller to determine the execution order of each model
  virtual bool is_source() const = 0;

  const std::string &get_name() const;
  void set_name(const std::string& name);
};
} // namespace mbd
