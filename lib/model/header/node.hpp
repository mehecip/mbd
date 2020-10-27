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

	// setting an ouput basically means that we have to write to the input ports we are connected to
	// so actually writing data on this ouput can be avoided to gain some performance
	template<typename T>
	void set_output(std::uint64_t index, const T& data) const
	{
		const auto& it = _connections.find(_out_ports[index].get());
		if (it == _connections.end()) 
			return; // no connections to this port -> nothing to update

		const auto& inputs = (*it).second;
		std::for_each(inputs.begin(), inputs.end(),
			[&](const auto& input)
			{
				input->template write_data<T>(data);
			});
	}

	template<typename T>
	const T& get_input(std::uint64_t index) const
	{
		return _in_ports[index]->read_data<T>();
	}

	template<typename T>
	void add_input(const std::string& name, const T& init_data)
	{
		auto p = std::make_unique<port>(name, port_dir_t::IN);
		p->set_data<T>(init_data);

		_in_ports.push_back(std::move(p));
	}

	template<typename T>
	void add_output(const std::string& name, const T& init_data)
	{
		auto p = std::make_unique<port>(name, port_dir_t::OUT);
		p->set_data<T>(init_data);

		_out_ports.push_back(std::move(p));
	}

private:
	std::vector<port_ptr_t> _in_ports;
	std::vector<port_ptr_t> _out_ports;

	std::unordered_map<port*, std::vector<port*>> _connections;

	connection_state validate(std::uint64_t this_out, const std::unique_ptr<node>& other, std::uint64_t other_in);
};

} // namespace mbd
