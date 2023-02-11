#pragma once

#include <AML/Algorithms/Root.hpp>

namespace aml
{
namespace algorithms
{

template<unsigned Steps = 0, class T> constexpr
auto taylor_sin(const T& val) noexcept
{
	auto out = val;
	aml::series<5, Steps, 2>(&out, [&, next = aml::cbr(val) / T(-6)](auto step) mutable {
		out += next;
		next *= aml::sqr(val) / T((step - 1) * step);
		next *= -1;
	});
	return out;
}
template<unsigned Steps = 0, class T> constexpr
auto taylor_cos(const T& val) noexcept
{
	auto out = T(1);
	aml::series<4, Steps, 2>(&out, [&, next = aml::sqr(val) / T(-2)](auto step) mutable {
		out += next;
		next *= aml::sqr(val) / T((step - 1) * step);
		next *= -1;
	});
	return out;
}

template<unsigned Steps = 100, class T> constexpr
auto taylor_asin(const T& val) noexcept
{
	auto out = val;
	aml::series<5, Steps, 2>(&out, 
		[&, nextval = aml::cbr(val), nextfrac = T(0.5)](auto step) mutable
	{
		out += nextfrac * (nextval / T(step - 2));
		nextval *= aml::sqr(val);
		nextfrac *= T(step - 2) / T(step - 1);
	});
	return out;
}

template<unsigned Steps = 0, class T> constexpr
auto approx_asin(const T& val) noexcept
{
	// https://stackoverflow.com/a/62855844

	constexpr auto half_pi_ = static_cast<T>(1.5707963267948966);

	const auto sqrt_val = newton_sqrt((T(1) - val) / T(2));

	return half_pi_ - (2 * taylor_asin<Steps>(sqrt_val));
}


}

}

