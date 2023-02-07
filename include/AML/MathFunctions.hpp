#pragma once

#include <AML/Algorithms/Power.hpp>
#include <AML/Algorithms/Newtons_method.hpp>

#include <cmath>

namespace aml {

template<class T> [[nodiscard]] constexpr
auto sqrt(const T& val) noexcept 
{
	using common = std::common_type_t<T, float>;

	if (AML_IS_CONSTANT_EVALUATED()) 
	{
		if (aml::equal(val, static_cast<common>(0))) return static_cast<common>(0);

		return aml::algorithms::raw_newtons_method<common>([&](auto x) {
			return static_cast<common>(0.5) * (x + (val / x));
		}, static_cast<common>(val));
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
