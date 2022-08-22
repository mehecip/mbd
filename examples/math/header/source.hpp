#pragma once
#include "model.hpp"

namespace mbd
{
namespace impl
{

template <typename T>
class source 
	: public model
{
public:
	source(const std::string& name, T init_val, std::uint64_t step_tick) : model(name), _init_value(init_val), _step_tick(step_tick) {};

	void build() override
	{
		add_output<T>(_name + "_OUT",  _init_value);
	}

	bool is_feedthrough() const override final
	{
		return false;
	}

	virtual void update(std::uint64_t tick) override = 0;

protected:
	T _init_value;
	std::uint64_t _step_tick;
};

template <typename T>
class const_source : public source<T>
{
public:
	const_source(const std::string& name, T init_val = T{}, std::uint64_t step_tick = 0ull)
		: source<T>(name, init_val, step_tick) {};

	void set_value(T val)
	{
		model::set_param(_param, val);	
	}

	void build() override
	{
		source<T>::build();
		model::add_param<T>(_param, T{});
	}

	void update(std::uint64_t tick) override
	{
		if(tick >= source<T>::_step_tick)
		{
			const T& val = model::get_param<T>(_param);
			model::set_output(0, val);
		}
		else
			model::set_output(0, source<T>::_init_value);
	}

private:
	const std::string _param = "const-val";

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
		model::set_output(0, source<T>::_init_value);

		if (tick >= source<T>::_step_tick)
			source<T>::_init_value += _step_val;
	}

private:
	T _step_val;
};

}
}