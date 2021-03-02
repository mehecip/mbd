#pragma once

#include <typeindex>

namespace mbd
{

struct i_data_type
{
	virtual ~i_data_type() {};
	virtual const std::type_index get_type() const = 0;
};


template<typename T>
struct data_type_wrapper : public i_data_type
{
	data_type_wrapper(T data) : _data(data) {};
	T _data;

	const std::type_index get_type() const override;
};

template<typename T>
inline const std::type_index data_type_wrapper<T>::get_type() const
{
	return typeid(T);
}

} // namespace mbd