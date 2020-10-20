#pragma once
#include "component.hpp"

namespace mbd
{
namespace impl
{

template <typename T>
class sink 
	:  public component
{
public:
	sink(const std::string& name) : component(name) {}

	void build() override
	{
		component::add_input<T>(component::_name + "_In", T{});
	}

	void update(std::uint64_t tick) override {}

	bool is_feedthrough() const override
	{
		return true;
	}

	T read()
	{
		return component::get_input<T>(0);
	}
};

}
}