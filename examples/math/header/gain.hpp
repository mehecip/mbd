#pragma once
#include "model.hpp"

namespace mbd
{
namespace impl
{

template <typename T>
class gain
	: public model
{
public:
	gain(const std::string& name, T gain) : model(name), _zero(T{}) 
	{
		model::add_input<T>(model::_name + "_In1", T{});
		model::add_output<T>(model::_name + "_Out1", T{});

		model::add_param<bool>(_overflow, false);
		model::add_param<T>(_gain, gain);
	}

	void update(std::uint64_t tick) override
	{
		const T in = model::get_input<T>(0);
		const T gain = model::get_param<T>(_gain);

		const T out = in * gain;
		
		check_overflow(in, out, gain);

		model::set_output<T>(0, std::move(out));
	}

	bool is_feedthrough() const override
	{
		return true;
	}

	bool has_overflown() const
	{
		return model::get_param<T>(_overflow);
	}

private:
	std::string _gain = "gain_factor";
	std::string _overflow = "overflow";
	const T _zero;

	void check_overflow(const T& in, const T& out, const T& gain)
	{
		if (in != _zero && gain != _zero)
		{
			if (out / gain != in)
			{
				model::set_param<bool>(_overflow, true);
				add_message(log_level::ERROR, "Overflow in " + _name + " for input = '" + std::to_string(in) + "' and gain = '" + std::to_string(gain) + "'");
			}
		}
			
	}
};

}
}