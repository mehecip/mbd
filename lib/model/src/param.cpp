#include "param.hpp"
#include "data_holder.hpp"

namespace mbd
{
param::param(const std::string &name) : data_holder(), _name(name) {}

param::param(param &&other) noexcept
    : data_holder(std::move(other)), _name(std::move(other._name))
{}
} // namespace mbd