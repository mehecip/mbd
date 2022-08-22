#pragma once
#include "model.hpp"

namespace mbd
{
namespace impl
{

template <typename T>
class sink 
	: public model
{
public:
	sink(const std::string& name) : model(name) 
	{
		model::add_input<T>(model::_name + "_In", T{});
	}

	void update(std::uint64_t tick) override {}

	bool is_feedthrough() const override
	{
		return true;
	}

	T read() const
	{
		return model::get_input<T>(0);
	}
};

}
}