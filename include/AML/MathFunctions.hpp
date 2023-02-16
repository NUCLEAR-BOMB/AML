#pragma once

#include <AML/Algorithms/Power.hpp>
#include <AML/Algorithms/Newtons_method.hpp>
#include <AML/Algorithms/Exp.hpp>
#include <AML/Algorithms/Trigonometry.hpp>
#include <AML/Algorithms/Root.hpp>
#include <AML/Algorithms/Log.hpp>

#include <cmath>

namespace aml {

template<class T> [[nodiscard]] constexpr
auto sqrt(const T& val) noexcept 
{
#if AML_HAS_CONSTEXPR_BUILTIN(__builtin_sqrt)
	return __builtin_sqrt(val);
#else
	using common = aml::common_type<T, float>;
	if (AML_IS_CONSTANT_EVALUATED()) {
		if (aml::equal(val, aml::zero)) return static_cast<common>(0);
		AML_DEBUG_VERIFY(val >= common(0), "The number must not be less than zero");

		return aml::algorithms::newton_sqrt(static_cast<common>(val));
	} else {
		return static_cast<common>(::std::sqrt(val));
	}
#endif
}

template<class T> [[nodiscard]] constexpr
auto cbrt(const T& val) noexcept
{
#if AML_HAS_CONSTEXPR_BUILTIN(__builtin_cbrt)
	return __builtin_cbrt(val);
#else
	using common = aml::common_type<T, float>;;
	if (AML_IS_CONSTANT_EVALUATED()) {
		if (aml::equal(val, aml::zero)) return common(0);

		return aml::algorithms::raw_newtons_method<common>([&](auto x) {
			return (val / sqr(x) + 2*x) / common(3);
		}, static_cast<common>(val * 0.001));
	} else {
		return static_cast<common>(::std::cbrt(static_cast<common>(val)));
	}
#endif
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
		static_assert(aml::always_false<Left, Right>, "Supports only integers values of Right");
	}
}

template<class T> [[nodiscard]] constexpr
auto exp(const T& val) noexcept
{
#if AML_HAS_CONSTEXPR_BUILTIN(__builtin_exp)
	return __builtin_exp(val);
#else
	using common = aml::common_type<T, float>;
	if (AML_IS_CONSTANT_EVALUATED()) {
		return aml::algorithms::exp_series(static_cast<common>(val));
	} else {
		return static_cast<common>(::std::exp(val));
	}
#endif
}

template<class T> [[nodiscard]] constexpr
auto sin(const T& val) noexcept
{
#if AML_HAS_CONSTEXPR_BUILTIN(__builtin_sin)
	return __builtin_sin(val);
#else
	using common = aml::common_type<T, float>;
	if (AML_IS_CONSTANT_EVALUATED()) {
		return aml::algorithms::sin_series(static_cast<common>(val));
	} else {
		return static_cast<common>(::std::sin(val));
	}
#endif
}
template<class T> [[nodiscard]] constexpr
auto cos(const T& val) noexcept
{
#if AML_HAS_CONSTEXPR_BUILTIN(__builtin_cos)
	return __builtin_cos(val);
#else
	using common = aml::common_type<T, float>;
	if (AML_IS_CONSTANT_EVALUATED()) {
		return aml::algorithms::cos_series(static_cast<common>(val));
	} else {
		return static_cast<common>(::std::cos(val));
	}
#endif
}
template<class T> [[nodiscard]] constexpr
auto tan(const T& val) noexcept
{
#if AML_HAS_CONSTEXPR_BUILTIN(__builtin_tan)
	return __builtin_tan(val);
#else
	using common = aml::common_type<T, float>;
	if (AML_IS_CONSTANT_EVALUATED()) {
		return aml::sin(val) / aml::cos(val);
	} else {
		return static_cast<common>(::std::tan(val));
	}
#endif
}
template<class T> [[nodiscard]] constexpr
auto asin(const T& val) noexcept
{
#if AML_HAS_CONSTEXPR_BUILTIN(__builtin_asin)
	return __builtin_asin(val);
#else
	using common = aml::common_type<T, float>;
	if (AML_IS_CONSTANT_EVALUATED()) {
		return aml::algorithms::optimize_asin(static_cast<common>(val), [](auto&& val) { return aml::algorithms::asin_series(val); });
	} else {
		return static_cast<common>(::std::asin(val));
	}
#endif
}

template<class T> [[nodiscard]] constexpr
auto acos(const T& val) noexcept
{
#if AML_HAS_CONSTEXPR_BUILTIN(__builtin_acos)
	return __builtin_acos(val);
#else
	using common = aml::common_type<T, float>;
	if (AML_IS_CONSTANT_EVALUATED()) {
		constexpr auto half_pi_ = static_cast<common>(1.5707963267948966);
		return half_pi_ - aml::asin(val);
	} else {
		return static_cast<common>(::std::acos(val));
	}
#endif
}

template<class T> [[nodiscard]] constexpr
auto atan(const T& val) noexcept
{
#if AML_HAS_CONSTEXPR_BUILTIN(__builtin_atan)
	return __builtin_atan(val);
#else
	using common = aml::common_type<T, float>;
	if (AML_IS_CONSTANT_EVALUATED()) {
		return aml::algorithms::euler_atan<150>(static_cast<common>(val));
	} else {
		return static_cast<common>(::std::atan(val));
	}
#endif
}

template<class Y_, class X_> [[nodiscard]] constexpr
auto atan2(const Y_& y, const X_& x) noexcept
{
#if AML_HAS_CONSTEXPR_BUILTIN(__builtin_atan2)
	return __builtin_atan2(y, x);
#else
	using common = aml::common_type<Y_, X_, float>;
	if (AML_IS_CONSTANT_EVALUATED()) {
		AML_DEBUG_VERIFY(!(aml::equal(y, aml::zero) && aml::equal(x, aml::zero)), "y and x must not equal zero");

		using flt_y = aml::common_type<Y_, float>;
		using flt_x = aml::common_type<X_, float>;
		return aml::algorithms::atan2<150>(static_cast<flt_y>(y), static_cast<flt_x>(x));
	} else {
		return static_cast<common>(::std::atan2(y, x));
	}
#endif
}

template<class First, class... Rest> [[nodiscard]] constexpr
auto hypot(const First& f, const Rest&... rest) noexcept 
{
	using common = aml::common_type<float, First, Rest...>;
#if AML_HAS_CONSTEXPR_BUILTIN(__builtin_hypot)
	if constexpr (sizeof...(rest) == 1) {
		return static_cast<common>(__builtin_hypot(f, rest...));
	}
#endif
	if constexpr (sizeof...(rest) == 0) {
		return static_cast<common>(f);
	}
	if (AML_IS_CONSTANT_EVALUATED() || sizeof...(rest) > 2) {
		return static_cast<common>(aml::sqrt(aml::sum_of(aml::sqr(f), aml::sqr(rest)...)));
	} else {
		return static_cast<common>(::std::hypot(f, rest...));
	}
}

}
