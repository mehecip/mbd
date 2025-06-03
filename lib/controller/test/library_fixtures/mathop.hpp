#pragma once
#include "model.hpp"

using namespace mbd;

namespace tst
{

template <typename T>
class mathop 
	: public model
{
public:
	mathop(const std::string& name) : model(name)
	{
		add_input<T>(_name + "_In1", T{});
		add_input<T>(_name + "_In2", T{});
		add_output<T>(_name + "_Out1", T{});
	}

	bool is_source() const override
	{
		return false;
	}

	virtual void update(std::uint64_t tick) override = 0;
};



template <typename T>
class add: public mathop <T>
{
public:
	add() : mathop<T>("add") {}

	void update(std::uint64_t tick) override
	{
		const T& in1 = model::get_input<T>(0);
		const T& in2 = model::get_input<T>(1);

		model::set_output<T>(0, T{ in1 + in2 });
	}
};


template <typename T>
class subtract : public mathop<T>
{
public:
	subtract() : mathop<T>("subtract") {}

	void update(std::uint64_t tick) override
	{
		const T& in1 = model::get_input<T>(0);
		const T& in2 = model::get_input<T>(1);

		model::set_output<T>(0, T{ in1 - in2 });
	}
};

template <typename T>
class div : public mathop<T>
{
public:
	div() : mathop<T>("div") {}

	void update(std::uint64_t tick) override
	{
		const auto& in1 = model::get_input<T>(0);
		const auto& in2 = model::get_input<T>(1);

		if (in2 != T{ 0 })
			model::set_output<T>(0, T{ in1 / in2 });
		else
			model::add_message(log_level::ERROR, "Division by zero.");
	}
};

template <typename T>
class mod : public mathop<T>
{
public:
	mod() : mathop<T>("mod") {}

	void update(std::uint64_t tick) override
	{
		const auto& in1 = model::get_input<T>(0);
		const auto& in2 = model::get_input<T>(1);

		model::set_output<T>(0, T{ in1 % in2 });
	}
};

template <typename T>
class multiply : public mathop<T>
{
public:
	multiply() : mathop<T>("multiply") {}

	void update(std::uint64_t tick) override
	{
		const auto& in1 = model::get_input<T>(0);
		const auto& in2 = model::get_input<T>(1);

		model::set_output<T>(0, T{ in1 * in2 });
	}
};

}