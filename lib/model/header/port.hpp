#pragma once
#include <string>
#include "data_holder.hpp"

namespace mbd
{

	enum class port_dir_t : bool
	{
		IN = 0,
		OUT = 1
	};

	class port : public data_holder
	{
	public:
		port(const std::string &name, port_dir_t d);
		port(port &&other) noexcept;

		~port() = default;

		const port_dir_t get_dir() const;

		void set_name(const std::string &name);

		void set_connected(bool flag);

		bool is_connected() const;

		const std::string &get_name() const;

	private:
		const port_dir_t _dir;
		bool _connected;

		std::string _name;
	};

} // namespace mbd
