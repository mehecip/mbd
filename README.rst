mbd
######


C++ Model Based Development/Engineering Library 


Usage - mbd::model
-------------------

Implement:

.. code:: C++

	#include "component.hpp"

	template <typename T>
	class sum
		: public component /* inherit from component class */
	{

	public:

		/* override build() to add the inputs and outputs */
		void build() override
		{
			component::add_input<T>("Input 1", T{});
			component::add_input<T>("Input 2", T{});

			component::add_output<T>("Output", T{});
		}
		
		/* override update() to implement the model logic */
		void update(std::uint64_t tick) override
		{
			const T in1 = component::get_input<T>(0);
			const T in2 = component::get_input<T>(1);

			component::set_output<T>(0, T{ in1 + in2 });
		}
		
		sum(const std::string& name) : component(name) {}
	}
	

Build:

.. code:: C++

	auto csrc = std::make_shared<const_source<double>>("Constant Source", 10.0, 0.0, 0);
	csrc->build();
	
	auto lsrc = std::make_shared<liniar_source<double>>("Liniar Source", 0.0, -0.1, 0);
	lsrc->build();
	
	auto sum = std::make_shared<add<double>>("Sum");
	sum->build();
	
	auto sink = std::make_shared<sink<double>>("Sink");
	sink->build();
	
	
Connect:

.. code:: C++

	csrc->connect(0, sum, 0);

	lsrc->connect(0, sum, 1);
	
	sum->connect(0, sink, 0);

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
		
		sum->update(i);
		
		sink->update(i);
	}



Usage - mbd::conntroller
------------------------

.. code:: C++
	using const_src_d_t = const_source<double>;
	using lin_src_d_t = liniar_source<double>;
	using add_d_t = add<double>;
	using sink_d_t = sink<double>;


	mbd::controller cntrl(message_callback);

	cntrl.register_model<const_src_d_t>("Constant Source", 10.0, 0.0, 0);
	cntrl.register_model<lin_src_f_t>("Liniar Source", 0.0, -0.1, 0);
	cntrl.register_model<add_d_t>("Sum");
	cntrl.register_model<sink_d_t>("Sink");

	cntrl.connect("Constant Source", 0, "Sum", 0);
	cntrl.connect("Liniar Source", 0, "Sum", 1);
	cntrl.connect("Converter", 0, "Sum", 1);
	cntrl.connect("Sum", 0, "Sink", 0);

	cntrl.excution_order();

	cntrl.run(10'000);

	auto sink_ = cntrl.get<sink_d_t>("Sink");

ToDO:
-----

Implement ``view``
