#pragma once
#include "port.hpp"
#include "data_type.hpp"

namespace mbd
{

port::port(const std::string& name, port_dir_t d) 
	: _dir(d), _name(name)
{}

const port_dir_t port::get_dir() const
{
	return _dir;
}

void port::set_name(const std::string& name)
{
	_name = name;
}

const std::string& port::get_name() const
{
	return _name;
}

const std::type_index port::get_data_type() const
{
	return _data->get_type();
}


void port::set_connected(bool flag)
{
	_connected = flag;
}

bool port::is_connected() const
{
	return _connected;
}

} // namespace mbd
