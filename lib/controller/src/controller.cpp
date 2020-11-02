#include <sstream>
#include <future>

#include "controller.hpp"

namespace mbd
{
controller::controller(const msg_callback_f& f) : _callback_f(f)
{}

bool controller::connect(const std::string& out_model, std::uint64_t out_idx, const std::string& in_model, std::uint64_t in_idx)
{
	const auto out = _models.find(out_model);
	if (out == _models.end())
		return false;

	const auto in = _models.find(in_model);
	if (in == _models.end())
		return false;

	if (!out->second->connect(out_idx, in->second, in_idx))
		return false;

	// store the info to determine the execution order
	_descendents[out_model].push_back(in_model);
	_ancestors[in_model].push_back(out_model);

	return true;
}

bool controller::excution_order()
{
	_arithmetic_loop.clear();
	_priority_vect.clear();
	
	{
		prio_map_t prio_map;
		std::queue<std::string> q;

		// add all the models that are not feedthrough with prio 0
		// they will act as starting points in the bfs algo
		std::for_each(_models.cbegin(), _models.cend(),
			[&](const auto& m)
			{
				if (!m.second->is_feedthrough())
				{
					prio_map[m.first] = 0;
					q.push(m.first);
				}
			});

		// set a maximum number of loops to be able to break in case of an arithmetic loop
		// max complexity V * V
		std::size_t loops = _models.size() * _models.size();

		// bfs 
		while (!q.empty())
		{
			auto model = q.front();
			q.pop();

			update_prio_map(model, prio_map);

			for (const auto& d : _descendents[model])
			{
				if (prio_map.find(d) == prio_map.end())
					q.push(d);
			}

			if (--loops == 0)
				break;
		}

		add_to_prio_vect(prio_map);

	} // prio_map and q not needed anymore

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

void controller::update_prio_map(const std::string& comp, prio_map_t& prio_map)
{
	if (prio_map.find(comp) != prio_map.end())
		return;

	std::uint64_t prio = 0;

	bool mapped_all_ancestors = true;
	for (const auto& a : _ancestors[comp])
	{
		if (prio_map.find(a) == prio_map.end())
		{
			mapped_all_ancestors = false;
			break;
		}

		prio = std::max(prio, prio_map[a]);
	}

	// only if all ancestors are present in the map, we update the prio of this model to max_prio(ancestors) + 1
	if (mapped_all_ancestors)
		prio_map[comp] = ++prio;
}

// store the final priority in _priority_vect since it will be in the order of priorities, so more efficient to iterate over
void controller::add_to_prio_vect(const prio_map_t& prio_map)
{
	// at most _comp.size() elements
	_priority_vect.resize(_models.size());

	for (const auto& m : prio_map)
		_priority_vect[m.second].push_back(_models[m.first].get());

	_priority_vect.erase(
		std::remove_if(_priority_vect.begin(), _priority_vect.end(),
			[](const auto& el) { return el.size() == 0; }
	), _priority_vect.end());
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
	for (const auto& el : _models)
	{
		bool found = false;

		const auto& model_name = el.first;
		const auto& model = el.second.get();
		for (const auto& models : _priority_vect)
		{
			if (std::find(models.begin(), models.end(), model) != models.end())
			{
				found = true;
				break;
			}
		}

		if (!found)
			_arithmetic_loop.push_back(model_name);
	}
}

} // namespace mbd
