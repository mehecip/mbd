#pragma once
#include <cstdint>
#include <string>

#include <memory>

#include <vector>
#include <unordered_map>

#include <unordered_set>

#include <algorithm>

#include <functional>

#include "node.hpp"
#include "log_level.hpp"
#include "connection_state.hpp"

namespace mbd
{

using node_ptr_t = std::unique_ptr<node>;
using msg_callbacks_t = std::function<void(log_level, const std::string&)>;

class component
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
	// this method is used by a controller to determin the execution order of the blocks
	virtual bool is_feedthrough() const = 0;

	virtual ~component() {};

	const std::string& get_name() const;

	bool connect(std::uint64_t this_out, const std::unique_ptr<component>& other, std::uint64_t other_in);

	bool disconnect(std::uint64_t this_out, const std::unique_ptr<component>& other, std::uint64_t other_in);

	void add_msg_callback(const msg_callbacks_t& f);

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
	T get_input(std::uint64_t index)
	{
		return _node->get_input<T>(index);
	}

	template<typename T>
	void set_output(std::uint64_t index, const T& data)
	{
		_node->set_output<T>(index, data);
	}

	void add_message(log_level lvl, const std::string& msg);

private:
	// a node is type which holds the ports and manages their connections and data
	// e.g. another layer of abstraction the component and ports
	const node_ptr_t _node;

	std::vector<msg_callbacks_t> _msg_callbacks;

	void log_connection_state(connection_state expected, connection_state current, const std::string& status);
};
} // namespace mbd
