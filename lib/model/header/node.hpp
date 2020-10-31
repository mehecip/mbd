#pragma once
#include <string>
#include <cstdint>
#include <memory>
#include <vector>
#include <unordered_map>
#include <algorithm>

#include "port.hpp"
#include "connection_state.hpp"

namespace mbd
{

using port_ptr_t = std::unique_ptr<port>;

class node
{
public:
	node() = default;
	~node() = default;

	connection_state connect(std::uint64_t this_out, const std::unique_ptr<node>& other, std::uint64_t other_in);

	connection_state disconnect(std::uint64_t this_out, const std::unique_ptr<node>& other, std::uint64_t other_in);

	template<typename T>
	void set_output(std::uint64_t index, const T& data) const;

	template<typename T>
	const T& get_input(std::uint64_t index) const;

	template<typename T>
	void add_input(const std::string& name, const T& init_data);

	template<typename T>
	void add_output(const std::string& name, const T& init_data);

private:
	std::vector<port_ptr_t> _in_ports;
	std::vector<port_ptr_t> _out_ports;

	// index represents the output port index and value is a vector of all connections from that port
	std::vector<std::vector<port*>> _output_connections;

	connection_state validate(std::uint64_t this_out, const std::unique_ptr<node>& other, std::uint64_t other_in);
};

// setting an ouput basically means that we have to write to the input ports we are connected to
// so actually writing data on this ouput can be avoided to gain some performance
template<typename T>
inline void node::set_output(std::uint64_t index, const T& data) const
{
	const auto& inputs = _output_connections[index];
	std::for_each(inputs.begin(), inputs.end(),
		[&](const auto& input)
		{
			input->template write_data<T>(data);
		});
}

template<typename T>
inline const T& node::get_input(std::uint64_t index) const
{
	return _in_ports[index]->read_data<T>();
}

template<typename T>
inline void node::add_input(const std::string& name, const T& init_data)
{
	auto p = std::make_unique<port>(name, port_dir_t::IN);
	p->set_data<T>(init_data);

	_in_ports.push_back(std::move(p));
}

template<typename T>
inline void node::add_output(const std::string& name, const T& init_data)
{
	auto p = std::make_unique<port>(name, port_dir_t::OUT);
	p->set_data<T>(init_data);

	_out_ports.push_back(std::move(p));
	_output_connections.push_back({});
}

} // namespace mbd
