#pragma once

#include <AML/Algorithms/Power.hpp>
#include <AML/Algorithms/Newtons_method.hpp>
#include <AML/Algorithms/Exp.hpp>
#include <AML/Algorithms/Trigonometry.hpp>
#include <AML/Algorithms/Root.hpp>

#include <cmath>

namespace aml {

template<class T> [[nodiscard]] constexpr
auto sqrt(const T& val) noexcept 
{
	using common = aml::common_type<T, float>;

	if (AML_IS_CONSTANT_EVALUATED()) 
	{
		if (aml::equal(val, static_cast<common>(0))) return static_cast<common>(0);

		return aml::algorithms::newton_sqrt(static_cast<common>(val));
	}
	else 
	{
		return static_cast<common>(
			::std::sqrt(
				static_cast<common>(val)
			)
		);
	}
}

template<class T> [[nodiscard]] constexpr
auto cbrt(const T& val) noexcept
{
	using common = aml::common_type<T, float>;;

	if (AML_IS_CONSTANT_EVALUATED())
	{
		if (aml::equal(val, static_cast<common>(0))) return static_cast<common>(0);

		return aml::algorithms::raw_newtons_method<common>([&](auto x) {
			return (val / sqr(x) + 2*x) / static_cast<common>(3);
		}, static_cast<common>(val * 0.001));
	}
	else
	{
		return static_cast<common>(
			::std::cbrt(
				static_cast<common>(val)
			)
		);
	}
}

template<class Left, class Right> [[nodiscard]] constexpr
auto pow(const Left& left, const Right& right) noexcept {
	if constexpr (std::is_integral_v<Right>) {
		if (aml::positive(right)) {
			return aml::algorithms::squaring_pow(left, right);
		} else {
			return (static_cast<Right>(1) / aml::algorithms::squaring_pow(left, right));
		}
	} else {
		static_assert(!sizeof(Left*), "Supports only integers values of Right");
	}
}

template<class T> [[nodiscard]] constexpr
auto exp(const T& val) noexcept
{
	using common = aml::common_type<T, float>;
	if (AML_IS_CONSTANT_EVALUATED())
	{
		return aml::algorithms::taylor_exp(val);
	}
	else
	{
		return static_cast<common>(
			::std::exp(
				static_cast<common>(val)
			)
		);
	}
}

template<class OutType = aml::selectable_unused, class First, class Second, class... Rest> [[nodiscard]] constexpr
auto dist(const First& first, const Rest&... rest) noexcept {
	auto sum = aml::sum_of(first, rest...);
	return aml::sqrt<OutType>(sum);
}

template<class Range, class First, class... Rest> [[nodiscard]] constexpr
bool in_range(const Range& range, const First& first, const Rest&... rest) noexcept {
	return (aml::dist(first, rest...) < range);
}

}
