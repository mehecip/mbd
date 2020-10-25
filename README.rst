mbd
######


C++ Model Based Development/Engineering Library 


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

	auto csrc = std::make_shared<const_source<double>>("Constant Source", 10.0, 0.0, 0);
	csrc->build();
	
	auto lsrc = std::make_shared<liniar_source<double>>("Liniar Source", 0.0, -0.1, 0);
	lsrc->build();
	
	auto sum_ = std::make_shared<sum<double>>("Sum");
	sum_->build();
	
	auto sink_ = std::make_shared<sink_<double>>("Sink");
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



Usage - mbd::conntroller
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

	cntrl.run(10'000);
	
Use:

.. code:: C++

	auto sink_ = cntrl.get<sink_d_t>("Sink");


ToDO:
-----

Implement ``view``
