#pragma once
#include "component.hpp"

namespace mbd
{
namespace impl
{

template <typename T>
class mathop 
	: public component
{
public:
	mathop(const std::string& name) : component(name) {}

	void build() override
	{
		add_input<T>(_name + "_In1", T{});
		add_input<T>(_name + "_In2", T{});

		add_output<T>(_name + "_Out1", T{});
	}

	bool is_feedthrough() const override
	{
		return true;
	}

	virtual void update(std::uint64_t tick) override = 0;
};



template <typename T>
class add: public mathop <T>
{
public:
	add(const std::string& name) : mathop<T>(name) {}

	void update(std::uint64_t tick) override
	{
		const T in1 = component::get_input<T>(0);
		const T in2 = component::get_input<T>(1);

		component::set_output<T>(0, T{ in1 + in2 });
	}
};



template <typename T>
class subtract : public mathop<T>
{
public:
	subtract(const std::string& name) : mathop<T>(name) {}

	void update(std::uint64_t tick) override
	{
		const T in1 = component::get_input<T>(0);
		const T in2 = component::get_input<T>(1);

		component::set_output<T>(0, T{ in1 - in2 });
	}
};

template <typename T>
class div : public mathop<T>
{
public:
	div(const std::string& name) : mathop<T>(name) {}

	void update(std::uint64_t tick) override
	{
		auto in1 = component::get_input<T>(0);
		auto in2 = component::get_input<T>(1);

		if (in2 != T{ 0 })
			component::set_output<T>(0, T{ in1 / in2 });
	}
};

template <typename T>
class mod : public mathop<T>
{
public:
	mod(const std::string& name) : mathop<T>(name) {}

	void update(std::uint64_t tick) override
	{
		auto in1 = component::get_input<T>(0);
		auto in2 = component::get_input<T>(1);

		component::set_output<T>(0, T{ in1 % in2 });
	}
};

template <typename T>
class multiply : public mathop<T>
{
public:
	multiply(const std::string& name) : mathop<T>(name) {}

	void update(std::uint64_t tick) override
	{
		auto in1 = component::get_input<T>(0);
		auto in2 = component::get_input<T>(1);

		component::set_output<T>(0, T{ in1 * in2 });
	}
};

}
}