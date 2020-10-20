#pragma once
#include "component.hpp"

namespace mbd
{
namespace impl
{

template <typename T>
class source 
	: public component
{
public:
	source(const std::string& name, T init_val, std::uint64_t step_tick) : component(name), _value(init_val), _step_tick(step_tick) {};

	void build() override
	{
		add_output<T>(_name + "_OUT",  _value);
	}

	bool is_feedthrough() const override final
	{
		return false;
	}

	virtual void update(std::uint64_t tick) override = 0;

protected:
	T _value;
	std::uint64_t _step_tick;
};

template <typename T>
class const_source : public source<T>
{
public:
	const_source(const std::string& name, T const_val, T init_val = T{}, std::uint64_t step_tick = 0ull)
		: source<T>(name, init_val, step_tick), _const_val(const_val) {};

	void set_value(T val)
	{
		_const_val = val;
	}

	void update(std::uint64_t tick) override
	{
		if(tick >= source<T>::_step_tick)
			component::set_output(0, _const_val);
		else
			component::set_output(0, source<T>::_value);
	}

private:
	T _const_val;
};

template <typename T>
class liniar_source : public source<T>
{
public:
	liniar_source(const std::string& name, T init_val = T{}, T step_val = T{}, std::uint64_t step_tick = 0ull)
		: source<T>(name, init_val, step_tick), _step_val(step_val) {};

	void set_param(T init_val, T step_val)
	{
		source<T>::_value = init_val;
		_step_val = step_val;
	}

	void update(std::uint64_t tick) override
	{
		component::set_output(0, source<T>::_value);

		if (tick >= source<T>::_step_tick)
			source<T>::_value += _step_val;
	}

private:
	T _step_val;
};

}
}