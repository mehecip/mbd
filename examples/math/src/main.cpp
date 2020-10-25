#include "model.hpp"
#include "log_level.hpp"

#include "source.hpp"
#include "mathop.hpp"
#include "sink.hpp"
#include "type_convertor.hpp"
#include "unit_delay.hpp"
#include "gain.hpp"

#include <iostream>

using namespace mbd;
using namespace mbd::impl;

void message_callback(log_level lvl, const std::string& msg)
{
	std::cout << level_info(lvl) << ": " << msg << "\n";
}

#if BUILD_CONTROLLER_EXAMPLES

#include "controller.hpp"
void controller_example()
{
	std::cout << "\n\n \t\t Controller Example \n\n";

	using const_src_d_t = const_source<double>;
	using lin_src_f_t = liniar_source<float>;
	using add_d_t = add<double>;
	using sink_d_t = sink<double>;
	using type_conv_f_to_d_t = type_convertor<float, double>;
	using gain_d_t = gain<double>;


	mbd::controller cntrl(message_callback);

	cntrl.register_model<const_src_d_t>("Constant Source", 10'000.0, -100.0, 10'001);
	cntrl.register_model<lin_src_f_t>("Liniar Source", -3.1415926f, 0.001f, 0);
	cntrl.register_model<type_conv_f_to_d_t>("Converter");
	cntrl.register_model<add_d_t>("Sum");
	cntrl.register_model<sink_d_t>("Sink");
	cntrl.register_model<gain_d_t>("Gain", 2.0);

	cntrl.connect("Constant Source", 0, "Sum", 0);
	cntrl.connect("Liniar Source", 0, "Converter", 0);
	cntrl.connect("Converter", 0, "Sum", 1);
	cntrl.connect("Sum", 0, "Gain", 0);
	cntrl.connect("Gain", 0, "Sink", 0);

	cntrl.excution_order();

	cntrl.run(1'000'000ull);



	auto sink_ = cntrl.get<sink_d_t>("Sink");
	std::cout << "Sink Value after execution: " << sink_->read() << "\n\n";
}

#endif

void example()
{

	std::cout << "\n\n \t\t Example w/o controller \n\n";

	/****************** CREATE ******************/

	std::uint64_t csrc_step_tick = 1000ull;
	double csrc_val = 1'000.0;
	double csrc_init_val = 0.0;
	auto csrc = std::make_shared<const_source<double>>("Constant Source", csrc_val, csrc_init_val, csrc_step_tick);
	csrc->add_msg_callback(message_callback);

	float lsrc_init_val = -110.0f;
	float lsrc_step_val = 1.0f;
	auto lsrc = std::make_shared<liniar_source<float>>("Liniar Source", lsrc_init_val, lsrc_step_val);
	lsrc->add_msg_callback(message_callback);

	auto type_conv = std::make_shared<type_convertor<float, double>>("Type Convertor");
	type_conv->build();
	type_conv->add_msg_callback(message_callback);

	auto sum = std::make_shared<add<double>>("Sum");
	sum->add_msg_callback(message_callback);

	double gain_val = 2.0;
	auto gain_ = std::make_shared<gain<double>>("Gain", 2.0);
	gain_->add_msg_callback(message_callback);

	auto sink_ = std::make_shared<sink<double>>("Sink");
	sink_->add_msg_callback(message_callback);

	/****************** BUILD ******************/

	csrc->build();
	lsrc->build();
	sum->build();
	gain_->build();
	sink_->build();

	/****************** CONNECT ******************/

	csrc->connect(0, sum, 0);
	lsrc->connect(0, type_conv, 0);
	type_conv->connect(0, sum, 1);
	sum->connect(0, gain_, 0);
	gain_->connect(0, sink_, 0);


	/****************** EXECUTE ******************/
	// exacution order is:
	// 0 -> Constant Source and Liniar Source
	// 1 -> Type Convertor
	// 2 -> Sum
	// 3 -> Gain
	// 4 -> Sink

	std::uint64_t ticks = 1'000'000ull;

	for (std::uint64_t i = 0; i < ticks; i++)
	{
		csrc->update(i);
		lsrc->update(i);

		type_conv->update(i);

		sum->update(i);

		gain_->update(i);

		sink_->update(i);
	}

	/****************** CHECK ******************/

	// (ticks - 1) because on the first tick liniar_source sets the init value as output and then updates the value for the next tick
	double expected_val = (
		csrc_val +
		(
			((double)(ticks - 1) * lsrc_step_val) + lsrc_init_val

			)
		) * gain_val;

	double actual_val = sink_->read();

	std::cout << "\n\nExpected sink value: " << expected_val << ". Actual value: " << actual_val << "\n\n";
}

int main()
{
	std::cout << "The Design: \n\n";
	std::cout << "|Constant Source| -------------------------------> |     | \n";
	std::cout << "                                                   | Sum | -------> | Gain | -------> | Sink | \n";
	std::cout << "|Liniar Source| -------> |Type Convertor| -------> |     | \n";
	std::cout << "\n\n\n\n";


	example();

#if BUILD_CONTROLLER_EXAMPLES
	controller_example();
#endif

	return 0;
}