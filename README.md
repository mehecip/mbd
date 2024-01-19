mbd
######


C++ Model Based Development/Engineering Library 
-----------------------------------------------

- fast: Design with 7 models executed 10.000.000 ticks in 1.55 sec on Ubuntu.
- easy to use: must implement only 2 methods: update() and is_feedthrough() in which you call add_input/add_output and get_input/set_output. 
- portable: Tested on Windows 10(Visual Studio 2019), Debian VM(GCC 8.3) and Raspbian(GCC 4.9).
- syncronous and asyncronous execution(based on execution order) available with the controller.


Clone and build:
-------------------


```Shell
	git clone https://github.com/mehecip/mbd.git
	cd mbd
	cmake -DBUILD_CONTROLLER=On -DBUILD_EXAMPLES=On .
	make f=Makefile
```

Usage - mbd::model
-------------------

Implement:

```c++

	#include "model.hpp"

	class gain : public model
	{
	public:

		/** Build your model: add inputs, outputs and parameters. */
		gain(const std::string& name, double gain) : model(name)
		{
			model::add_input<double>("In 0", 0.0);
			model::add_output<double>("Out 0", 0.0);

			model::add_param<double>("gain_factor", gain);
		}

		/** Update your model: read inputs/parameters and set outputs/parameters. */
		void update(std::uint64_t tick) override
		{
			const double in = model::get_input<double>(0);
			const double gain = model::get_param<double>("gain_factor");
			
			model::set_output<double>(0, in * gain);
		}

		/** Let the controller know if the model behaves as a source. */
		bool is_feedthrough() const override
		{
			return true;
		}
	};
	
```
Register:

```c++

	mbd::lib my_lib("My Lib");
	my_lib.register_model<gain>("Times Pi", 3.1415);
```

Build:

```c++

	auto gain_ = my_lib.build_model("Times Pi");
	auto src_ = my_lib.build_model("Liniar Source");
	auto sink_ = my_lib.build_model("Sink");
```
	
Connect:

```c++

	mbd::end_point src_0{src_, 0, port_dir_t::OUT};
	mbd::end_point gain_0{gain_, 0, port_dir_t::IN};

	auto [state, src_to_gain] = connection::build(src_0, gain_0);

	/**************************************************************
		| Liniar Source |0>-------->0| Gain |0>-------->0| Sink | 
	***************************************************************/
```
Execute (in the correct order):

```c++	

	for (std::uint64_t i = 0; i < 10; ++i)
	{
		src_->update(i);
		gain_->update(i);
		sink_->update(i);
	}
```


Usage - mbd::controller
------------------------

Create the controller:

```c++

	#include "controller.hpp"
	
	void message_callback(log_level lvl, const std::string& msg)
	{
		std::cout << level_info(lvl) << ": " << msg << "\n";
	}
	
	mbd::controller cntrl(message_callback);
```
Add the models:

```c++

	cntrl.add_library(my_lib);

	cntrl.add_model("My Lib", "Times Pi");
	cntrl.add_model("My Lib", "Liniar Source");
	cntrl.add_model("My Lib", "Sink");
```

Connect the models:

```c++

	cntrl.connect("Liniar Source", 0, "Times Pi", 0);
	cntrl.connect("Times Pi", 0, "Sink", 0);

	/**************************************************************
		| Liniar Source |0>-------->0| Gain |0>-------->0| Sink | 
	***************************************************************/
```
	
Find algebraic loops:

```c++

	std::size_t n_loops = cntrl.find_algebraic_loops();
```
Calculate execution order and run all models:

```c++

	// syncronous
	cntrl.run(10'000);
	
	// or asyncronous
	cntrl.run_async(10'000);
```
Get:

```c++

	auto sink_ = cntrl.get<sink>("Sink");
	double value = sink_->read();
```
ToDO:
-----

Implement ``view``
