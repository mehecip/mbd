#pragma once

#include "model.hpp"

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

namespace mbd
{

class lib
{
private:
    using factory_t = std::function<model::ptr_t(const std::string& name)>;
    
public:
	lib(const std::string& name);

	template<typename M, typename ... Args>
	inline void register_model(const std::string& name, Args&&... args);

	model::ptr_t build(const std::string& name) const;

	const std::vector<std::string>& get_model_names() const;

private:
	std::unordered_map<std::string, factory_t> _factories;
	std::vector<std::string> _model_names;
	std::string _name;
};

template<typename M, typename ... Args>
inline void lib::register_model(const std::string& name, Args&&... args)
{
	if (_factories.find(name) != _factories.end())
		return;

	const factory_t& factory = [&](const std::string model_name) {
		return std::make_unique<M>(model_name, std::forward<Args>(args)...);
	};

	_factories[name] = std::move(factory);
	_model_names.push_back(name);
}

}