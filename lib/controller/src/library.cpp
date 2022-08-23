#include "library.hpp"
#include "model.hpp"

namespace mbd
{

    lib::lib(const std::string &name) : _name(name) {}

    model::ptr_t lib::build(const std::string &name) const
    {
        const auto &factory = _factories.find(name);

        if (factory != _factories.end())
            return factory->second(name);

        return nullptr;
    }

    const std::vector<std::string> &lib::get_model_names() const
    {
        return _model_names;
    }

}