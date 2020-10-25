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
	gain(const std::string& name, T gain) : model(name), _gain(gain), _overflow(false), _zero(T{}) {}

	void build() override
	{
		model::add_input<T>(model::_name + "_In1", T{});

		model::add_output<T>(model::_name + "_Out1", T{});
	}

	void update(std::uint64_t tick) override
	{
		const T in = model::get_input<T>(0);
		const T out = in * _gain;
		
		check_overflow(in, out);

		model::set_output<T>(0, std::move(out));
	}

	bool is_feedthrough() const override
	{
		return true;
	}

	bool has_overflown() const
	{
		return _overflow;
	}

private:
	T _gain;
	bool _overflow;
	const T _zero;

	void check_overflow(const T& in, const T& out)
	{
		if (in != _zero && _gain != _zero)
		{
			if (out / _gain != in)
			{
				_overflow = true;
				add_message(log_level::ERROR, "Overflow in " + _name + " for input = '" + std::to_string(in) + "' and gain = '" + std::to_string(_gain) + "'");
			}
		}
			
	}
};

}
}