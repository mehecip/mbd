#pragma once
#include "model.hpp"

using namespace mbd;

namespace tst
{

template <typename T>
class gain
	: public model
{
public:
	gain(T gain = {}) : model("gain"), _zero(T{}) 
	{
		model::add_input<T>(model::_name + "_In1", T{});
		model::add_output<T>(model::_name + "_Out1", T{});

		model::add_param<bool>(_overflow, false);
		model::add_param<T>(_gain, gain);
	}

	void set_value(const T& val)
	{
		model::set_param<T>(_gain, val);
	}

	void update(std::uint64_t) override
	{
		const T& in = model::get_input<T>(0);
		const T& gain = model::get_param<T>(_gain);

		const T& out = in * gain;
		
		check_overflow(in, out, gain);

		model::set_output<T>(0, out);
	}

	bool is_source() const override
	{
		return false;
	}

	bool has_overflown() const
	{
		return model::get_param<bool>(_overflow);
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
