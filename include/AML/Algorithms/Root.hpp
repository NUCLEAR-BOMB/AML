#pragma once

#include <AML/Algorithms/Newtons_method.hpp>

namespace aml
{
namespace algorithms
{

template<class T> constexpr
auto newton_sqrt(const T& val) noexcept
{
	if (aml::equal(val, T(0))) return T(0);

	return raw_newtons_method<T>([&](auto x) {
		return T(0.5) * (x + (val / x));
	}, val * T(0.1));
}

}

}