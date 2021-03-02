#include <sstream>
#include <future>

#include "controller.hpp"

namespace mbd
{
controller::controller(const msg_callback_f& f) : _callback_f(f)
{}

bool controller::connect(const std::string& out_model, std::uint64_t out_idx, const std::string& in_model, std::uint64_t in_idx)
{
	const auto& out = _models.find(out_model);
	if (out == _models.end())
		return false;

	const auto& in = _models.find(in_model);
	if (in == _models.end())
		return false;

	if (!out->second->connect(out_idx, in->second, in_idx))
		return false;

	bool is_start_model = !out->second->is_feedthrough();
	_graph.add_edge(out_model, in_model, is_start_model);

	return true;
}

bool controller::excution_order()
{
	add_to_prio_vect(_graph.priority());

	log_prio();

	find_arithmetic_loop();

	log_arithmetic_loop();

	return _arithmetic_loop.size() == 0ull;
}

void controller::run(std::uint64_t ticks)
{
	for (std::uint64_t t = 0; t < ticks; ++t)
	{
		// call update in the order of the prios
		for (const auto& models : _priority_vect)
		{
			for (const auto& model : models)
				model->update(t);
		}
	}
}

void controller::run_async(std::uint64_t ticks)
{
	for (std::uint64_t t = 0; t < ticks; ++t)
	{
		// call update in the order of the prios
		for (const auto& models : _priority_vect)
		{
			std::vector<std::future<void>> futures;

			// launch update() for all the models with the current priority
			for (const auto& model : models)
				futures.push_back(std::async(std::launch::async, &model::update, model, t));

			// wait for all executions for the current priority to finish
			for (const auto& f : futures)
				f.wait();
		}
	}
}


// store the final priority in _priority_vect since it will be in the order of priorities, so more efficient to iterate over
void controller::add_to_prio_vect(const prio_map_t& prio_map)
{
	_priority_vect.clear();

	// at most _comp.size() elements
	_priority_vect.resize(_models.size());

	for (const auto& m : prio_map)
		_priority_vect[m.second].push_back(_models[m.first].get());

	_priority_vect.erase(
		std::remove_if(_priority_vect.begin(), _priority_vect.end(),
			[](const auto& el) { return el.size() == 0; })
		, _priority_vect.end());

	_priority_vect.shrink_to_fit();
}


void controller::log_prio()
{
	std::ostringstream ss;
	for (std::uint64_t idx = 0; idx < _priority_vect.size(); ++idx)
	{
		ss << "Update priority " << idx << ": ";
		for (const auto& model : _priority_vect[idx])
			ss << " [" << model->get_name() << "]";
		ss << "\n\t";
	}

	_callback_f(log_level::INFO, ss.str());
}

void controller::log_arithmetic_loop()
{
	if (_arithmetic_loop.size() == 0)
		return;

	std::ostringstream ss;

	ss << "It seems there is an arithmetic loop: ";
	for (const auto& model_name : _arithmetic_loop)
		ss << "[" << model_name << "] ";
	ss << "\n";

	_callback_f(log_level::ERROR, ss.str());
}

void controller::find_arithmetic_loop()
{
	_arithmetic_loop.clear();

	for (const auto& el : _models)
	{
		const auto& model_name = el.first;
		if (!_graph.is_prioritized(model_name))
			_arithmetic_loop.push_back(model_name);
	}
}

} // namespace mbd
