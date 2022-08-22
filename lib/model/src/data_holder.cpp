#include "data_holder.hpp"
#include "data_type.hpp"

namespace mbd
{
    data_holder::data_holder(data_holder &&other) noexcept
        : _data(std::move(other._data)) {}

    const std::type_index data_holder::get_data_type() const
    {
        return _data->get_type();
    }
}