#include "model.hpp"
#include "log_level.hpp"

#include "source.hpp"
#include "mathop.hpp"
#include "sink.hpp"
#include "type_convertor.hpp"
#include "unit_delay.hpp"
#include "gain.hpp"

#include <iostream>

#include <chrono>

using namespace mbd;
using namespace mbd::impl;


class Timer
{
public:
	Timer() : beg_(clock_::now()) {}
	void reset() { beg_ = clock_::now(); }

	double elapsed_sec() const {
		return std::chrono::duration_cast<second_>
			(clock_::now() - beg_).count();
	}

	double elapsed_microsec() const {
		return std::chrono::duration_cast<microsec_>
			(clock_::now() - beg_).count();
	}

	double elapsed_mili() const {
		return std::chrono::duration_cast<milisec_>
			(clock_::now() - beg_).count();
	}

private:
	typedef std::chrono::high_resolution_clock clock_;
	typedef std::chrono::duration<double, std::ratio<1>> second_;
	typedef std::chrono::duration<double, std::ratio<1, 1'000>> milisec_;
	typedef std::chrono::duration<double, std::ratio<1, 1'000'000>> microsec_;

	std::chrono::time_point<clock_> beg_;
};


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

	Timer t;

	mbd::controller cntrl(message_callback);

	cntrl.register_model<const_src_d_t>("Constant Source", 10'000.0, -100.0, 10'001);

	cntrl.register_model<lin_src_f_t>("Liniar Source", -3.1415926f, 0.001f, 0);
	cntrl.register_model<type_conv_f_to_d_t>("Converter");
	cntrl.register_model<add_d_t>("Sum");
	cntrl.register_model<sink_d_t>("Sink");
	cntrl.register_model<gain_d_t>("Gain", 2.0);


	std::cout << "Creating and registring durration: " << t.elapsed_microsec() << " microseconds." << std::endl;

	t.reset();

	cntrl.connect("Constant Source", 0, "Sum", 0);
	cntrl.connect("Liniar Source", 0, "Converter", 0);
	cntrl.connect("Converter", 0, "Sum", 1);
	cntrl.connect("Sum", 0, "Gain", 0);
	cntrl.connect("Gain", 0, "Sink", 0);

	std::cout << "Connecting durration: " << t.elapsed_microsec() << " microseconds. " << std::endl;

	t.reset();

	cntrl.excution_order();

	std::cout << "Execution order: " << t.elapsed_microsec() << " microseconds. " << std::endl;

	t.reset();

	std::uint64_t ticks = 10'000'000;
	cntrl.run(ticks);

	std::cout << ticks << " ticks executed in " << t.elapsed_sec() << " seconds. Average tick duration " << t.elapsed_microsec() / ticks << " microseconds." << std::endl;

	auto sink_ = cntrl.get<sink_d_t>("Sink");
	std::cout << "Sink Value after execution: " << sink_->read() << "\n\n";
}

#endif

void example()
{

	std::cout << "\n\n \t\t Example w/o controller \n\n";

	/****************** BUILD ******************/

	std::uint64_t csrc_step_tick = 1000ull;
	double csrc_val = 1'000.0;
	double csrc_init_val = 0.0;
	auto csrc = std::make_unique<const_source<double>>("Constant Source", csrc_val, csrc_init_val, csrc_step_tick);
	csrc->add_msg_callback(message_callback);

	float lsrc_init_val = -110.0f;
	float lsrc_step_val = 1.0f;
	auto lsrc = std::make_unique<liniar_source<float>>("Liniar Source", lsrc_init_val, lsrc_step_val);
	lsrc->add_msg_callback(message_callback);

	std::unique_ptr<model> type_conv = std::make_unique<type_convertor<float, double>>("Type Convertor");
	type_conv->add_msg_callback(message_callback);

	std::unique_ptr<model> sum = std::make_unique<add<double>>("Sum");
	sum->add_msg_callback(message_callback);

	double gain_val = 2.0;
	std::unique_ptr<model> gain_ = std::make_unique<gain<double>>("Gain", 2.0);
	gain_->add_msg_callback(message_callback);

	std::unique_ptr<model> sink_ = std::make_unique<sink<double>>("Sink");
	sink_->add_msg_callback(message_callback);



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

	double actual_val = static_cast<sink<double>*>(sink_.get())->read();

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