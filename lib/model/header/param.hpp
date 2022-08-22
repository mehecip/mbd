#pragma once
#include <string>

#include "data_holder.hpp"

namespace mbd
{
	class param : public data_holder
	{
	public:
		param(const std::string &name);
		param(param &&other) noexcept;

		~param() = default;

		void set_name(const std::string &name);
		const std::string &get_name() const;

	private:
		std::string _name;
	};

} // namespace mbd
