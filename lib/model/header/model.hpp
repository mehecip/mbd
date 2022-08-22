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
	model(const std::string& n);

	// called when building
	// use to add inpus/outputs to your model
	virtual void build() = 0;

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

	// // for build() you would like to add inputs/outputs
	// template<typename T>
	// void add_input(const std::string& name, const T& init_data);

	// template<typename T>
	// void add_output(const std::string& name, const T& init_data);

	// // for update() you would like to put/get data
	// template<typename T>
	// const T& get_input(std::uint64_t index) const;

	// template<typename T>
	// void set_output(std::uint64_t index, const T& data) const;

private:
	// node is type which holds the ports and manages their connections and data
	// e.g. another layer of abstraction between the model and ports
	// mbd::node _node;

	void log_connection_state(connection_state expected, connection_state current, const std::string& status);
};

// template<typename T>
// inline void model::add_input(const std::string& name, const T& init_data)
// {
// 	_node.add_input<T>(name, init_data);
// }

// template<typename T>
// inline void model::add_output(const std::string& name, const T& init_data)
// {
// 	_node.add_output<T>(name, init_data);
// }

// template<typename T>
// inline const T& model::get_input(std::uint64_t index) const
// {
// 	return _node.get_input<T>(index);
// }

// template<typename T>
// inline void model::set_output(std::uint64_t index, const T& data) const
// {
// 	_node.set_output<T>(index, data);
// }

} // namespace mbd
