mbd
######


C++ Model Based Development/Engineering Library 

- fast: Design with 6 models executed 10.000.000 ticks in 2.3 sec on Raspberry Pi 3, 0.6 sec on Windows 10 and 0.4 sec on Debian VM.
- easy to use
- portable: Tested on Windows 10(Visual Studio 2019), Debian VM(GCC 8.3) and Raspbian(GCC 4.9).
- syncronous and asyncronous execution(based on execution order) available in the controller.


Clone and build:
-------------------

.. code:: console

	git clone https://github.com/mehecip/mbd.git
	cd mbd
	cmake -DBUILD_CONTROLLER=On -DBUILD_EXAMPLES=On .
	make f=Makefile


Usage - mbd::model
-------------------

Implement:

.. code:: C++

	#include "model.hpp"

	template <typename T>
	class sum
		: public model /* inherit from mbd::model class */
	{

	public:

		/* override build() to add the inputs and outputs */
		void build() override
		{
			model::add_input<T>("Input 1", T{});
			model::add_input<T>("Input 2", T{});

			model::add_output<T>("Output", T{});
		}
		
		/* override update() to implement the logic */
		void update(std::uint64_t tick) override
		{
			const T in1 = model::get_input<T>(0);
			const T in2 = model::get_input<T>(1);

			model::set_output<T>(0, T{ in1 + in2 });
		}
		
		sum(const std::string& name) : model(name) {}
	}
	

Build:

.. code:: C++

	auto csrc = std::make_unique<const_source<double>>("Constant Source", 10.0, 0.0, 0);
	csrc->build();
	
	auto lsrc = std::make_unique<liniar_source<double>>("Liniar Source", 0.0, -0.1, 0);
	lsrc->build();
	
	auto sum_ = std::make_unique<sum<double>>("Sum");
	sum_->build();
	
	auto sink_ = std::make_unique<sink_<double>>("Sink");
	sink_->build();
	
	
Connect:

.. code:: C++

	csrc->connect(0, sum_, 0);

	lsrc->connect(0, sum_, 1);
	
	sum_->connect(0, sink_, 0);

	/**************************************************************
	| Constant Source |0>-------->0|     |
                                       | Sum |0>------->0| Sink |
          | Liniar Source |0>-------->1|     |
	***************************************************************/

Execute (in the correct order):

.. code:: C++	

	for (std::uint64_t i = 0; i < 10'000; ++i)
	{
		csrc->update(i);
		lsrc->update(i);
		
		sum_->update(i);
		
		sink_->update(i);
	}



Usage - mbd::controller
------------------------

Create the controller:

.. code:: C++

	#include "controller.hpp"
	
	mbd::controller cntrl(message_callback);

Register the models:

.. code:: C++

	using const_src_d_t = const_source<double>;
	using lin_src_d_t = liniar_source<double>;
	using add_d_t = add<double>;
	using sink_d_t = sink<double>; 
	
	cntrl.register_model<const_src_d_t>("Constant Source", 10.0, 0.0, 0);
	cntrl.register_model<lin_src_f_t>("Liniar Source", 0.0, -0.1, 0);
	cntrl.register_model<add_d_t>("Sum");
	cntrl.register_model<sink_d_t>("Sink");
	
Connect the models:

.. code:: C++

	cntrl.connect("Constant Source", 0, "Sum", 0);
	cntrl.connect("Liniar Source", 0, "Sum", 1);
	cntrl.connect("Sum", 0, "Sink", 0);

	/**************************************************************
	| Constant Source |0>-------->0|     |
                                       | Sum |0>------->0| Sink |
          | Liniar Source |0>-------->1|     |
	***************************************************************/
	
Calculate the execution order:

.. code:: C++

	cntrl.excution_order();

Execute:

.. code:: C++

	// syncronous
	cntrl.run(10'000);
	
	// or asyncronous
	// cntrl.run_async(10'000);
	
Get:

.. code:: C++

	auto sink_ = cntrl.get<sink_d_t>("Sink");
	double value = sink_->read();


ToDO:
-----

Implement ``view``
