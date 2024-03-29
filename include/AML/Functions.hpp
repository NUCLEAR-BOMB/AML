/** @file */
#pragma once

#include <AML/Tools.hpp>
#include <limits>
#include <string_view>

#ifdef AML_LIBRARY
	#define AML_LIBRARY_FUNCTIONS
#else
	#error AML library is required
#endif

namespace aml {

/**
	@brief Checks if the processor has a big endianness
*/
inline
bool is_big_endian() noexcept {
	static const union {
		uint32_t i;
		char c[4];
	} bint = { 0x01020304 };
	return (bint.c[0] == 1);
}


namespace detail
{
	template<class Operation, class First, class... Ts> [[nodiscard]] constexpr
	decltype(auto) compare(First&& f, Ts&&... vals) noexcept
	{
		constexpr bool is_ref =
			(std::is_reference_v<First> || (std::is_reference_v<Ts> || ...))
			&& (std::is_same_v<std::remove_const_t<First>, std::remove_const_t<Ts>> && ...);
	
		if constexpr (is_ref)
		{
			using ptr_type_ = std::conditional_t<
				std::is_const_v<std::remove_reference_t<First>> || (std::is_const_v<std::remove_reference_t<Ts>> || ...),
				const aml::remove_cvref<First>*,
				aml::remove_cvref<First>*
			>;

			ptr_type_ out = &f;
			((out = (Operation{}(vals, *out) ? &vals : out)), ...);
			return *out;
		}
		else 
		{
			using common = aml::common_type<aml::remove_cvref<First>, aml::remove_cvref<Ts>...>;

			common v = static_cast<common>(f);
			((v = (Operation{}(vals, v) ? static_cast<common&&>(vals) : v)), ...);
			return v;
		}
	}
}

/**
	@brief Maximum element from input variadic arguments
*/
template<class First, class... Ts> [[nodiscard]] constexpr
decltype(auto) max(First&& f, Ts&&... vals) noexcept
{
	return detail::template compare<aml::greater>(std::forward<First>(f), std::forward<Ts>(vals)...);
}

template<class First, class... Ts> [[nodiscard]] constexpr
decltype(auto) min(First&& f, Ts&&... vals) noexcept
{
	return detail::template compare<aml::less>(std::forward<First>(f), std::forward<Ts>(vals)...);
}


/**
	@brief The sum of the input variadic arguments
*/
template<class First, class... Rest> [[nodiscard]] constexpr
auto sum_of(First&& first, Rest&&... rest) noexcept {
	decltype(first + (rest + ...)) out = std::forward<First>(first);
	([&] {
		out += std::forward<Rest>(rest);
	}(), ...);
	return out;
}

/**
	@brief Square the number. @f$ x^2 @f$
*/
template<class OutType = aml::selectable_unused, class T> [[nodiscard]] constexpr
auto sqr(const T& val) noexcept {
	return aml::selectable_convert<OutType>(val * val);
}

template<class T> [[nodiscard]] constexpr
auto cbr(const T& val) noexcept {
	return (val * val * val);
}

/**
	@brief Absolute value or modulus of a number. @f$ |x| @f$
*/
template<class T> [[nodiscard]] constexpr
auto abs(const T& val) noexcept {
	if constexpr (std::is_unsigned_v<T>) {
		return val;
	} else {
#if AML_HAS_CONSTEXPR_BUILTIN(__builtin_fabs)
		if constexpr (std::is_same_v<T, float>) {
			return __builtin_fabsf(val);
		} else if constexpr (std::is_same_v<T, double>) {
			return __builtin_fabs(val);
		} else if constexpr (std::is_same_v<T, long double>) {
			return __builtin_fabsl(val);
		}
#endif
		return (val < static_cast<T>(0)) ? (-val) : (val);
	}
}

struct equal_fn
{
template<class Left, class Right> [[nodiscard]] constexpr
bool operator()(const Left& left, const Right& right) const noexcept
{
	if constexpr (std::is_floating_point_v<Left> && std::is_floating_point_v<Right>)
	{
		using common = std::common_type_t<Left, Right>;

		const auto m = aml::max(static_cast<common>(1), static_cast<common>(aml::abs(left)), static_cast<common>(aml::abs(right)));
		return (aml::abs(left - right)) <= (std::numeric_limits<common>::epsilon() * m);
	}
	else if constexpr (std::is_arithmetic_v<Left> && std::is_arithmetic_v<Right>) {
		using common = std::common_type_t<Left, Right>;
		return (static_cast<common>(left) == static_cast<common>(right));
	}
	else if constexpr (std::is_invocable_r_v<bool, aml::equal_to, Left, Right>) {
		return (left == right);
	}
	else if constexpr (std::is_constructible_v<Left, decltype(right)> 
					   && std::is_invocable_r_v<bool, aml::equal_to, Left, Left>) {
		return (left == static_cast<Left>(right));
	}
	else if constexpr (std::is_constructible_v<Right, decltype(left)>
					   && std::is_invocable_r_v<bool, aml::equal_to, Right, Right>) {
		return (static_cast<Right>(left) == right);
	}
	else if constexpr (std::is_invocable_r_v<bool, aml::equal_to, decltype(aml::unwrap(std::declval<Left>())), decltype(aml::unwrap(std::declval<Right>()))>) {
		return (aml::unwrap(left) == aml::unwrap(right));
	}
	else {
		static_assert(aml::always_false<Left, Right>, "Left and Right types do not have a equal operator");
	}
}

};

/**
	@brief Checks if the @p left and @p right are equal
	@details Can compare a floating point value

	@see #aml::not_equal
*/
inline constexpr equal_fn equal;

#ifndef AML_NO_CUSTOM_OPERATORS
namespace detail {
	template<class T>
	struct equal_op_proxy {
		const T& v;
	};

	template<class T> constexpr
	const detail::equal_op_proxy<T> operator<(const T& left, const equal_fn&) { return { left }; }

	template<class Left, class Right> constexpr
	decltype(auto) operator>(const detail::equal_op_proxy<Left>& left, const Right& right) {
		return aml::equal(left.v, right);
	}
}

#endif


/**
	@brief Checks if the @p left and @p right are not equal

	@see #aml::equal
*/
template<class Left, class Right> [[nodiscard]] constexpr
bool not_equal(const Left& left, const Right& right) noexcept {
	return !aml::equal(left, right);
}

/**
	@brief Checks if the @p val is zero
	@details Can also check types that casts to #aml::zero
*/
template<class T> [[nodiscard]] constexpr
bool is_zero(const T& val) noexcept {
	return aml::equal(val, static_cast<T>(aml::zero));
}

/**
	@brief Distance between scalars
*/
template<class OutType = aml::selectable_unused, class Left, class Right> [[nodiscard]] constexpr
auto dist_between(const Left& left, const Right& right) noexcept {
	return aml::abs<OutType>(left - right);
}

/**
	@brief Checks if @p val is odd
*/
template<class T> [[nodiscard]] /** @cond */ AML_FORCEINLINE /** @endcond */ constexpr
bool odd(const T& val) noexcept 
{
	static_assert(!std::is_floating_point_v<T>, "T must not be a floating point number");
	return static_cast<bool>((val % static_cast<T>(2)) != static_cast<T>(0));
}

/**
	@brief Checks if @p val is even
*/
template<class T> [[nodiscard]] /** @cond */ AML_FORCEINLINE /** @endcond */ constexpr
bool even(const T& val) noexcept {
	return !aml::odd(val);
}

/**
	@brief Checks if @p val is negative
*/
template<class T> [[nodiscard]] /** @cond */ AML_FORCEINLINE /** @endcond */ constexpr
bool negative(const T& val) noexcept {
	if constexpr (std::is_unsigned_v<T>) {
		return false;
	} else {
		return (val < static_cast<T>(0));
	}
}

template<class T> [[nodiscard]] constexpr
auto negate_if(T&& val, bool cond)
{
	return (cond ? -std::forward<T>(val) : std::forward<T>(val));
}

/**
	@brief Checks if @p val is positive
*/
template<class T> [[nodiscard]] /** @cond */ AML_FORCEINLINE /** @endcond */ constexpr
bool positive(const T& val) noexcept {
	return !aml::negative(val);
}

/**
	@brief Rounds down the @p val
*/
template<class OutType = aml::selectable_unused, class T> [[nodiscard]] constexpr
decltype(auto) floor(T&& val) noexcept
{
	if constexpr (std::is_integral_v<T>) { 
		return aml::selectable_convert<OutType>(val);
	} 
	else if constexpr (std::is_floating_point_v<T>) { 
		const std::int_fast64_t t = static_cast<std::int_fast64_t>(val);
		using conv = aml::selectable_type<OutType, T>;
		return static_cast<conv>(t - (static_cast<T>(t) > val ? 1 : 0));
	} 
	else {
		static_assert(aml::always_false<T>, "Flooring not supported type");
	}
}

/**
	@brief Rounds up the @p val
*/
template<class OutType = aml::selectable_unused, class T> [[nodiscard]] constexpr
decltype(auto) ceil(T&& val) noexcept
{
	if constexpr (std::is_integral_v<T>) {
		return aml::selectable_convert<OutType>(val);
	}
	else if constexpr (std::is_floating_point_v<T>) {
		const auto t = static_cast<std::int_fast64_t>(val);
		using conv = aml::selectable_type<OutType, T>;
		return static_cast<conv>(t + (static_cast<T>(t) < val ? 1 : 0));
	} 
	else {
		static_assert(aml::always_false<T>, "Ceiling not supported type");
	}
}

/**
	@brief Rounds @p val to the nearest value
*/
template<class OutType = aml::selectable_unused, class T> [[nodiscard]] constexpr
decltype(auto) round(T&& val) noexcept
{
	if constexpr (std::is_integral_v<T>) {
		return aml::selectable_convert<OutType>(val);
	} 
	else if constexpr (std::is_floating_point_v<T>) {
		return (val >= static_cast<T>(0)) ? 
			aml::floor<OutType>(val + static_cast<T>(0.5)) 
		  : aml::floor<OutType>(val - static_cast<T>(0.5));
	} 
	else {
		static_assert(aml::always_false<T>, "Rounding not supported type");
	}
}

template<class T> [[nodiscard]] constexpr
bool is_gteq_zero(const T& val) noexcept
{
	if constexpr (std::is_unsigned_v<T>) {
		return val;
	} else {
		return (val >= static_cast<T>(aml::zero));
	}
}

template<unsigned Start = 0, decltype(Start) Steps = 0, decltype(Start) Step = 1, class T, class Fun> constexpr
void series(const T* val_ptr, Fun&& fun) noexcept
{
	auto stepc = Start;
	while (true)
	{
		auto last = *val_ptr;

		fun(stepc); // change val

		if (aml::equal(last, *val_ptr)) break;
		if constexpr (Steps != 0) {
			if (stepc == (Start + (Steps * Step))) break;
		}

		stepc += Step;
	}
}

[[nodiscard]] constexpr
auto string_to_double(const char* const str, std::size_t size) noexcept
{
	using flt_type = double;

	flt_type out = 0.;

	const auto* const str_end = str + size;

	const auto dot_it = [&]() {
		auto it = str;
		for (; it != str_end; ++it) {
			if (*it == '.') return it;
		}
		return it;
	}();
	
	auto it = (dot_it - 1);
	flt_type pos = 1.0;
	for (;; --it, pos *= 10.0)
	{
		out += static_cast<flt_type>(*it - '0') * pos;
		if (it == str) break;
	}
	if (dot_it == str_end) return out;

	pos = 1.0 / 10.0;
	it = (dot_it + 1);
	for (; it != str_end; ++it, pos /= 10.0)
	{
		out += static_cast<flt_type>(*it - '0') * pos;
	}

	return out;
}

}
