#pragma once

#include <AML/Functions.hpp>

namespace aml
{
namespace algorithms
{

// ln_series2 is better
template<unsigned Steps = 0, class T> constexpr
auto ln_series1(const T& val) noexcept
{
	// 0 < val <= 2
	const auto x = val - 1;
	auto out = x;
	aml::series<2, Steps, 1>(&out, [&, next = -aml::sqr(x)](auto step) mutable {
		out += (next / step);
		next *= -x;
	});
	return out;
}

template<unsigned Steps = 0, class T> constexpr
auto ln_series2(const T& val) noexcept
{
	// 0 < val
	const auto x = (val - 1) / (val + 1); 
	
	auto out = x;
	aml::series<3, Steps, 2>(&out, [&, next = aml::cbr(x)](auto step) mutable {
		out += (next / step);
		next *= aml::sqr(x);
	});
	return 2*out;
}

}

}