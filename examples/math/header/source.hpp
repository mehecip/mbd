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
	source(const std::string& name, T init_val, std::uint64_t step_tick) 
		: model(name) 
	{
		add_output<T>(_name + "_OUT",  init_val);

		model::add_param<T>(_init_val, init_val);
		model::add_param<T>(_step_tick, step_tick);
	}


	bool is_feedthrough() const override final
	{
		return false;
	}

	virtual void update(std::uint64_t tick) override = 0;

protected:
	std::string _init_val = "init_value";
	std::string _step_tick = "step_tick";
};

template <typename T>
class const_source : public source<T>
{
public:
	const_source(const std::string& name, const T& val, T init_val = T{}, std::uint64_t step_tick = 0ull)
		: source<T>(name, init_val, step_tick)	
	{
		model::add_param<T>(_param, val);
	}

	void set_value(T val)
	{
		model::set_param(_param, val);	
	}

	void update(std::uint64_t tick) override
	{
		const T& step_tick = model::get_param<T>(source<T>::_step_tick);
		if(tick >= step_tick)
		{
			const T& val = model::get_param<T>(_param);
			model::set_output(0, val);
		}
		else
		{
			const T& val = model::get_param<T>(source<T>::_init_val);
			model::set_output(0, val);
		}
	}

private:
	const std::string _param = "const-val";

};

template <typename T>
class liniar_source : public source<T>
{
public:
	liniar_source(const std::string& name, T init_val = T{}, T step_val = T{}, std::uint64_t step_tick = 0ull)
		: source<T>(name, init_val, step_tick) 
	{
		model::add_param<T>(_current_val, init_val);
		model::add_param<T>(_step_val, step_val);
	}

	void set_param(T curr_val, T step_val)
	{
		model::set_param(_current_val, curr_val);
		model::set_param(_step_val, step_val);
	}

	void update(std::uint64_t tick) override
	{
		const T& current_val = model::get_param<T>(_current_val);
		const T& step_tick = model::get_param<T>(source<T>::_step_tick);

		if(tick >= step_tick)
		{
			const T& step_val = model::get_param<T>(_step_val);
			const T& next_val = current_val + step_val;

			model::set_output(0, next_val);
			model::set_param(_current_val, next_val);
		}
		else
		{
			model::set_output(0, current_val);
		}
	}

private:
	std::string _step_val = "step_value";
	std::string _current_val = "current_value";
};

}
}