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
class component; //fw declaration
using component_ptr_t = std::unique_ptr<component>;
using node_ptr_t = std::unique_ptr<node>;

class component 
	: public msg_dispatcher
{

public:
	component(const std::string& n);

	// called when building
	// use to add inpus/outputs to your component
	virtual void build() = 0;

	// called when executing
	virtual void update(std::uint64_t tick) = 0;

	// a component is feedthrough if the CURRENT value of an input determines the CURRENT value of an output
	// e.g.:
	// sum -> true
	// source -> false
	// unit delay -> false (the CURRENT value of the input determines the NEXT value of the output)
	// this method is used by a controller to determine the execution order of each component
	virtual bool is_feedthrough() const = 0;

	virtual ~component() {};

	const std::string& get_name() const;

	bool connect(std::uint64_t this_out, const component_ptr_t& other, std::uint64_t other_in);

	bool disconnect(std::uint64_t this_out, const component_ptr_t& other, std::uint64_t other_in);

protected:
	std::string _name;
	std::string _validator_msg;

	// for build() you would like to add inputs/outputs
	template<typename T>
	void add_input(const std::string& name, const T& init_data)
	{
		_node->add_input<T>(name, init_data);
	}

	template<typename T>
	void add_output(const std::string& name, const T& init_data)
	{
		_node->add_output<T>(name, init_data);
	}

	// for update() you would like to put/get data
	template<typename T>
	const T& get_input(std::uint64_t index) const
	{
		return _node->get_input<T>(index);
	}

	template<typename T>
	void set_output(std::uint64_t index, const T& data) const
	{
		_node->set_output<T>(index, data);
	}

private:
	// a node is type which holds the ports and manages their connections and data
	// e.g. another layer of abstraction the component and ports
	const node_ptr_t _node;

	void log_connection_state(connection_state expected, connection_state current, const std::string& status);
};
} // namespace mbd
