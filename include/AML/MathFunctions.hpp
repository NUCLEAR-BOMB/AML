#pragma once

#include <AML/MathAlgorithms.hpp>

AML_NAMESPACE

template<class OutType = aml::selectable_unused, class T> [[nodiscard]] constexpr
auto sqrt(const T& val) noexcept 
{
	T start_val;

	if		(val < T(10000))  start_val = T(44.72135955)  + T(0.011180339887)   * (val - T(2000));
	else if (val < T(200000)) start_val = T(1000)		  + T(0.0005)			* (val - T(1000000));
	else					  start_val = T(2236.0679775) + T(0.00022360679775) * (val - T(5000000));

	return aml::selectable_convert<OutType>(aml::algorithms::newton_sqrt<true>(val, start_val));
}

template<class Left, class Right> [[nodiscard]] constexpr
auto pow(const Left& left, const Right& right) noexcept {
	if constexpr (std::is_integral_v<Right>) {
		if (aml::positive(right)) {
			return algorithms::binary_pow(left, right);
		} else {
			return (static_cast<Right>(1) / algorithms::binary_pow(left, right));
		}
	} else {
		static_assert(!sizeof(Left*), "Supports only integers values of Right");
	}
}

template<class OutType = aml::selectable_unused, class First, class Second, class... Rest> [[nodiscard]] constexpr
auto dist(const First& first, const Second& second, const Rest&... rest) noexcept {
	auto sum = aml::sum_of(first, rest...);
	return aml::sqrt<OutType>(sum);
}

template<class Range, class First, class... Rest> [[nodiscard]] constexpr
bool in_range(const Range& range, const First& first, const Rest&... rest) noexcept {
	return (aml::dist(first, rest...) < range);
}



AML_NAMESPACE_END
