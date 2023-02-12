#pragma once

#include <AML/Functions.hpp>

namespace aml
{
namespace algorithms
{

template<unsigned Steps = 0, class T> constexpr
auto exp_series(const T& val) noexcept
{
	auto out = 1 + val;
	aml::series<3, Steps, 1>(&out, [&, next = aml::sqr(val) / T(2)](auto step) mutable {
		out += next;
		next *= val / T(step);
	});
	return out;
}


}

}