#pragma once

#include <AML/MathAlgorithms.hpp>

AML_NAMESPACE

template<class T> [[nodiscard]] constexpr
auto sqrt(const T& val) noexcept {

	T start_val;

	if		(val < T(10000))  start_val = T(44.72135955)  + T(0.011180339887)   * (val - T(2000));
	else if (val < T(200000)) start_val = T(1000)		  + T(0.0005)			* (val - T(1000000));
	else					  start_val = T(2236.0679775) + T(0.00022360679775) * (val - T(5000000));

	return aml::algorithms::newton_sqrt(val, start_val);
}



AML_NAMESPACE_END
