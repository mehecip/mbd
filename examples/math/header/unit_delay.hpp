#pragma once
#include "component.hpp"

namespace mbd
{
namespace impl
{

template <typename T>
class unit_delay
	: public component
{
public:
	unit_delay(const std::string& name, T init_val = T{}) : component(name), _value(init_val) {};

	void build() override
	{
		add_input<T>(_name + "_IN", _value);
		add_output<T>(_name + "_OUT", _value);
	}

	void update() override
	{
		set_output<T>(0, T{ _value });
		_value = get_input<T>(0);
	}

	void set_init_val(T init_val)
	{
		_value = init_val;
	}

	bool is_feedthrough() const override final
	{
		return false;
	}

private:
	T _value;
};

}
}