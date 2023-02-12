#pragma once

#include <AML/Algorithms/Root.hpp>

namespace aml
{
namespace algorithms
{

template<unsigned Steps = 0, class T> constexpr
auto sin_series(const T& val) noexcept
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
auto cos_series(const T& val) noexcept
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
auto asin_series(const T& val) noexcept
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
auto fast_asin(const T& val) noexcept
{
	// https://stackoverflow.com/a/62855844

	constexpr auto half_pi_ = static_cast<T>(1.5707963267948966);

	constexpr auto calc = [](const auto& v) {
		return half_pi_ - (2 * asin_series<Steps>( newton_sqrt((T(1) - v) / T(2)) ));
	};

	if (val >= T(0)) {
		return calc(val);
	} else {
		return -calc(-val);
	}
}

template<unsigned Steps = 100, class T> constexpr
auto atan_series(const T& val) noexcept
{
	auto out = val;
	aml::series<3, Steps, 2>(&out, [&, next = -aml::cbr(val)](auto step) mutable {
		out += (next / T(step));
		next *= aml::sqr(val);
		next *= -1;
	});
	return out;
}

template<unsigned Steps = 100, class T> constexpr
auto fast_atan(const T& val) noexcept
{
	return 2 * atan_series<Steps>(val / (1 + newton_sqrt(1 + aml::sqr(val))) );
}

template<unsigned Steps = 100, class Y_, class X_> constexpr
auto atan2(const Y_& y, const X_& x) noexcept
{
	using common = aml::common_type<Y_, X_>;
	constexpr auto pi_ = static_cast<common>(3.141592653589793);
	constexpr auto half_pi_ = pi_ / 2;

	const bool is_x_zero = aml::equal(x, aml::zero);

	return 
		!is_x_zero ? fast_atan<Steps>(y / x) : common(0)
	  + (1 - 2*(y < 0))*(pi_*(x < 0) + half_pi_*is_x_zero);
}


}

}
