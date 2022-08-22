#pragma once
#include <cstdint>
#include <string>
#include <memory>

#include "node.hpp"
#include "log_level.hpp"
#include "connection_state.hpp"
#include "msg_dispatcher.hpp"

namespace mbd
{

class model 
	: public msg_dispatcher, node
{
public:
	using ptr_t = std::unique_ptr<model>;

public:

	// must be used to add inpus/outputs to your model
	model(const std::string& n);

	// called when executing
	virtual void update(std::uint64_t tick) = 0;

	// a model is feedthrough if the CURRENT value of an input determines the CURRENT value of an output
	// e.g.:
	// sum -> true
	// source -> false
	// unit delay -> false (the CURRENT value of the input determines the NEXT value of the output)
	// this method is used by a controller to determine the execution order of each model
	virtual bool is_feedthrough() const = 0;

	virtual ~model() {};

	const std::string& get_name() const;

	bool connect(std::uint64_t this_out, const model::ptr_t& other, std::uint64_t other_in);

	bool disconnect(std::uint64_t this_out, const model::ptr_t& other, std::uint64_t other_in);

protected:
	std::string _name;
	std::string _validator_msg;

private:
	void log_connection_state(connection_state expected, connection_state current, const std::string& status);
};
} // namespace mbd
