#include "component.hpp"
#include "log_level.hpp"

#include "source.hpp"
#include "mathop.hpp"
#include "sink.hpp"
#include "type_convertor.hpp"
#include "unit_delay.hpp"
#include "gain.hpp"


#include <iostream>
#include <sstream>


using namespace mbd;
using namespace mbd::impl;

void message_callback(log_level lvl, const std::string& msg)
{
	std::cout << level_info(lvl) << ":" << msg << "\n";
}

void test()
{

	/****************** CREATE ******************/

	std::uint64_t csrc_step_tick = 100ull;
	double csrc_val = 1'000.0;
	double csrc_init_val = 0.0;
	std::unique_ptr<component> csrc = std::make_unique<const_source<double>>("Constant Source", csrc_val, csrc_init_val, csrc_step_tick);
	csrc->add_msg_callback(message_callback);

	float liniar_src_init_val = 0.0;
	float liniar_src_step_val = -1.0f;
	auto lsrc = std::make_unique<liniar_source<float>>("Liniar Source", liniar_src_init_val, liniar_src_step_val);
	lsrc->add_msg_callback(message_callback);

	std::unique_ptr<component> type_conv = std::make_unique<type_convertor<float, double>>("Type Convertor");
	type_conv->build();
	type_conv->add_msg_callback(message_callback);

	std::unique_ptr<component> sum = std::make_unique<add<double>>("Sum");
	sum->add_msg_callback(message_callback);

	double gain_val = 2.0;
	std::unique_ptr<component> gain_ = std::make_unique<gain<double>>("Gain", 2.0);
	gain_->add_msg_callback(message_callback);

	std::unique_ptr<component> sink_ = std::make_unique<sink<double>>("Sink");
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

	std::uint64_t ticks = 10'001;
	
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

	std::ostringstream ostr;

	// (ticks - 1) because on the first tick liniar_source sets the init value as output and then updates the value for the next tick
	double expected_val = (
								csrc_val + 
								(
									((double)(ticks - 1) * liniar_src_step_val) + liniar_src_init_val
									
								)
							) * gain_val;

	double actual_val = static_cast<sink<double>*>(sink_.get())->read();

	ostr << "\n\nExecution status: \n\n";
	ostr << "Expected sink value: " << expected_val << ". Actual value: " << actual_val << "\n\n";


	std::cout << ostr.str() << std::endl;
}

int main()
{	
	std::ostringstream ostr;
	ostr << "The model: \n\n";
	ostr << "|Constant Source| -------------------------------> |     | \n";
	ostr << "                                                   | Sum | -------> | Gain | -------> | Sink | \n";
	ostr << "|Liniar Source| -------> |Type Convertor| -------> |     | \n";
	ostr << "\n\n\n\n";
	std::cout << ostr.str() << std::endl;

	ostr.str("");

	test();
	return 0;
}