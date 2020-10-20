mbd
######


C++ Model Based Development/Engineering Library 


Usage
-----

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

	std::unique_ptr<component> csrc = std::make_unique<mbd::const_source<double>>("Constant Source", 10.0, 0.0, 0);
	csrc->build();
	
	std::unique_ptr<component> lsrc = std::make_unique<mbd::liniar_source<double>>("Liniar Source", 0.0, -0.1);
	lsrc->build();
	
	std::unique_ptr<component> sum_ = std::make_unique<mbd::sum<double>>("Sum");
	sum->build();
	
	std::unique_ptr<component> sink = std::make_unique<mbd::sink<double>>("Sink");
	sink->build();
	
	
Connect:

.. code:: C++

	csrc->connect(0, sum, 0);

	lsrc->connect(0, sum, 1);
	
	sum->connect(0, sink, 0);

	/*
	|Constant Source| -------> |     |
							   | Sum | -------> | Sink |
	 |Liniar Source|  -------> |     |
	*/

Execute (in the correct order):

.. code:: C++	

	for (std::uint64_t i = 0; i < 10'000; i++)
	{
		csrc->update(i);
		lsrc->update(i);
		
		sum->update(i);
		
		sink->update(i);
	}


	
ToDO:
-----

Implement ``controller`` and ``view``