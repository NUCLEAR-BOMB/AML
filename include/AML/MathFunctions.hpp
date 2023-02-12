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
#if AML_HAS_BUILDIN(__buildin_sqrt)
	return __buildin_sqrt(val);
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
#if AML_HAS_BUILDIN(__buildin_cbrt)
	return __buildin_cbrt(val);
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
#if AML_HAS_BUILDIN(__buildin_exp)
	return __buildin_exp(val);
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
#if AML_HAS_BUILDIN(__buildin_sin)
	return __buildin_sin(val);
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
#if AML_HAS_BUILDIN(__buildin_cos)
	return __buildin_cos(val);
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
#if AML_HAS_BUILDIN(__buildin_tan)
	return __buildin_tan(val);
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
#if AML_HAS_BUILDIN(__buildin_asin)
	return __buildin_asin(val);
#else
	using common = aml::common_type<T, float>;
	if (AML_IS_CONSTANT_EVALUATED()) {
		return aml::algorithms::fast_asin(static_cast<common>(val));
	} else {
		return static_cast<common>(::std::asin(val));
	}
#endif
}

template<class T> [[nodiscard]] constexpr
auto acos(const T& val) noexcept
{
#if AML_HAS_BUILDIN(__buildin_acos)
	return __buildin_acos(val);
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
#if AML_HAS_BUILDIN(__buildin_atan)
	return __buildin_atan(val);
#else
	using common = aml::common_type<T, float>;
	if (AML_IS_CONSTANT_EVALUATED()) {
		return aml::algorithms::fast_atan<150>(static_cast<common>(val));
	} else {
		return static_cast<common>(::std::atan(val));
	}
#endif
}

template<class Y_, class X_> [[nodiscard]] constexpr
auto atan2(const Y_& y, const X_& x) noexcept
{
#if AML_HAS_BUILDIN(__buildin_atan2)
	return __buildin_atan2(y, x);
#else
	using common = aml::common_type<Y_, X_, float>;
	if (AML_IS_CONSTANT_EVALUATED()) {
		AML_DEBUG_VERIFY(!aml::equal(y, aml::zero) && !aml::equal(x, aml::zero), "y and x must not equal zero");

		using flt_y = aml::common_type<Y_, float>;
		using flt_x = aml::common_type<X_, float>;
		return aml::algorithms::atan2<150>(static_cast<flt_y>(y), static_cast<flt_x>(x));
	} else {
		return static_cast<common>(::std::atan2(y, x));
	}
#endif
}

template<class First, class... Rest> [[nodiscard]] constexpr
auto hypot(First&& f, Rest&&... rest) noexcept {
	return aml::sqrt(aml::sum_of(std::forward<First>(f), std::forward<Rest>(rest)...));
}

}
