#pragma once
#include <queue>
#include <unordered_map>
#include <vector>

namespace mbd
{

template <typename T>
class prio_graph
{
public:
	prio_graph() : _nr_of_vertices(0)
	{}

	~prio_graph() = default;

	prio_graph(prio_graph&&) = delete;
	prio_graph(const prio_graph&) = delete;

	prio_graph& operator=(prio_graph&&) = delete;
	prio_graph& operator=(const prio_graph&) = delete;

	void add_edge(T from, T to, bool is_start_point);

	const std::unordered_map<T, std::uint64_t>& priority();

	bool is_prioritized(const T& v);

private:
	std::unordered_map<T, std::vector<T>> _descendents_map;
	std::unordered_map<T, std::vector<T>> _ancestors_map;
	std::unordered_map<T, std::uint64_t> _prio_map;
	std::uint64_t _nr_of_vertices;

	void update_prio_map(const T& v);
};

template <typename T>
inline void prio_graph<T>::update_prio_map(const T& v)
{
	if (is_prioritized(v))
		return;

	std::uint64_t prio = 0;

	bool mapped_all_ancestors = true;
	for (const auto& a : _ancestors_map[v])
	{
		if (_prio_map.find(a) == _prio_map.end())
		{
			mapped_all_ancestors = false;
			break;
		}

		prio = std::max(prio, _prio_map[a]);
	}

	// only if all ancestors are present in the map, we update the prio of this vertex
	if (mapped_all_ancestors)
		_prio_map[v] = ++prio;
}

template <typename T>
inline void prio_graph<T>::add_edge(T from, T to, bool is_start_point)
{
	_descendents_map[from].push_back(to);
	_ancestors_map[to].push_back(from);

	if (is_start_point)
		_prio_map[from] = 0;

	++_nr_of_vertices;
}

template <typename T>
inline const std::unordered_map<T, std::uint64_t>& prio_graph<T>::priority()
{
	std::queue<T> q;

	// add the starting verteces to the queue
	std::for_each(_prio_map.begin(), _prio_map.end(),
		[&](const auto& el)
		{
			if (el.second == 0)
				q.push(el.first);
		});


	std::uint64_t max_loops = _nr_of_vertices * _nr_of_vertices;

	// bfs 
	while (!q.empty())
	{
		auto v = q.front();
		q.pop();

		update_prio_map(v);

		// update the queue with all the descendands 
		for (const auto& d : _descendents_map[v])
			if(!is_prioritized(d))
				q.push(d);

		if (--max_loops == 0ull)
			break;
	}

	return _prio_map;
}

template <typename T>
inline bool prio_graph<T>::is_prioritized(const T& v)
{
	return _prio_map.find(v) != _prio_map.end();
}

}