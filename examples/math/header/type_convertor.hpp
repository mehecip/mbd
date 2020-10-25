#pragma once

#include "model.hpp"

#include <limits>
#include <typeinfo>

namespace mbd
{
namespace impl
{

template <typename IN, typename OUT>
class type_convertor
	: public model
{
private:
	bool _overflow;

public:
	type_convertor(const std::string& name) : model(name), _overflow(false) {}

	void build() override
	{
		add_input<IN>(_name + "_IN", IN{});
		add_output<OUT>(_name + "_OUT", OUT{});
	}

	void update(std::uint64_t tick) override
	{
		convert();
	}

	bool is_feedthrough() const override
	{
		return true;
	}

private:
	template<typename I = IN, typename O = OUT>
	using is_same_t = std::enable_if_t<std::is_same<I, O>::value>;

	template<typename I = IN, typename O = OUT>
	using is_not_same_t = std::enable_if_t<!std::is_same<I, O>::value>;


	// if same type, move the data from input to output
	template<typename I = IN, typename O = OUT>
	constexpr is_same_t<I, O>	convert()
	{
		set_output<O>(0, std::move(get_input<I>(0)));
	}

	template<typename I = IN, typename O = OUT>
	constexpr is_not_same_t<I, O> convert()
	{
		const I in = get_input<I>(0);
		set_output<O>(0, static_cast<O>(in));
	}
};

}
}
