#pragma once
#include <string>
#include <memory>
#include <typeindex>

#include "data_type.hpp"

namespace mbd
{

enum class port_dir_t : bool
{
	IN = 0,
	OUT = 1
};

class port
{
public:
	port(const std::string& name, port_dir_t d);

	~port() = default;

	const port_dir_t get_dir() const;

	void set_name(const std::string& name);

	void set_connected(bool flag);
	
	bool is_connected() const;

	const std::string& get_name() const;

	const std::type_index get_data_type() const;

	template<typename T>
	void set_data(const T& data)
	{
		_data = std::make_unique<data_type_wrapper<T>>(data);
	}

	template<typename T>
	void write_data(const T& data) const
	{
		static_cast<data_type_wrapper<T>*>(_data.get())->_data = data;
	}

	template<typename T>
	const T& read_data() const
	{
		return static_cast<data_type_wrapper<T>*>(_data.get())->_data;
	}

private:
	const port_dir_t _dir;
	bool _connected;

	std::string _name;
	std::unique_ptr<i_data_type> _data;
};

} // namespace mbd
