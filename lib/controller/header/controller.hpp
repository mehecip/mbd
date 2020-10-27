#pragma once
#include <queue>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <string>
#include <functional>

#include "model.hpp"

namespace mbd
{
using model_map_t = std::unordered_map<std::string, model_ptr_t> ;

using conn_map_t = std::unordered_map<std::string, std::vector<std::string>>;
using prio_map_t = std::unordered_map<std::string, std::uint64_t>;

class controller
{
public:
	controller(const msg_callback_f& f);
	~controller() = default;

	controller(controller&&) = delete;
	controller(const controller&) = delete;

	controller& operator=(controller&&) = delete;
	controller& operator=(const controller&) = delete;

	// creates, builds and stores a model of type M with name and args
	template<typename M, typename ... Args>
	void register_model(const std::string& name, Args&&... args)
	{
		auto c = std::make_unique<M>(name, std::forward<Args>(args)...);
		c->add_msg_callback(_callback_f);
		c->build();

		_models[name] = std::move(c);
	}

	// connects the out_idx of out_model to the in_idx of in_model
	bool connect(const std::string& out_model, std::uint64_t out_idx, const std::string& in_model, std::uint64_t in_idx);

	// calculates and prints the execution order for all connected models
	// returns true if execution is posible
	// or false otherwise (arithmetic loop detected)
	bool excution_order();

	// syncronous execution(in the order from execution order)
	void run(std::uint64_t ticks);

	// asyncronous execution(in the order from execution order)
	// all the models with the same prio order would be executed at the same time with std::async
	void run_async(std::uint64_t ticks);

	// get a T ptr to the model with key = name
	// or nullptr if T and model type dont match
	// or nullptr if model with key = name is not found in the map
	template<typename T>
	T* get(const std::string& name)
	{
		const auto& it = _models.find(name);
		if (it == _models.end())
			return nullptr;

		return dynamic_cast<T*>(it->second.get());
	}

private:
	msg_callback_f _callback_f;

	model_map_t _models;
	conn_map_t _descendents, _ancestors;

	// the index is the prio at which to call update()
	// the values are all the obj that need to be updated at that time
	std::vector<std::vector<model*>> _priority_vect;
	std::vector<std::string> _arithmetic_loop;

	// sets the prio for comp as:
	// P[comp] = max(P[Ancestor 1], P[Ancestor 2], ... , P[Ancestor N]) + 1
	void update_prio_map(const std::string& comp, prio_map_t& prio_map);

	// stores the final priority in _priority_vect since it will be in the order of priorities, so more efficient to iterate over
	void add_to_prio_vect(const prio_map_t& prio_map);

	void log_prio();

	// an arithmetic loop is a loop for which the execution order cannot be computed
	void find_arithmetic_loop();

	void log_arithmetic_loop();
};


}