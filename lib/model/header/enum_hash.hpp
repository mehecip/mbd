#pragma once
#include <cstdio>

struct enum_class_hash
{
	template <typename T>
	std::size_t operator()(T t) const
	{
		return static_cast<std::size_t>(t);
	}
};
