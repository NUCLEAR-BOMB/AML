/** @file */
#pragma once

#include <AML/Tools.hpp>
#include <limits>

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

template<class OutType = aml::selectable_unused, class First> [[nodiscard]] constexpr
decltype(auto) max(First&& first) noexcept {
	return aml::selectable_convert<OutType>(std::forward<First>(first));
}

template<class OutType = aml::selectable_unused, class First, class Second> [[nodiscard]] constexpr
decltype(auto) max(First&& first, Second&& second) noexcept {
	return aml::selectable_convert<OutType>((first > second) ? (std::forward<First>(first)) : (std::forward<Second>(second)));
}

/**
	@brief Maximum element from input variadic arguments
*/
template<class OutType = aml::selectable_unused, class First, class Second, class... Rest> [[nodiscard]] constexpr
auto max(First&& first, Second&& second, Rest&&... rest) noexcept 
{
	using outtype = std::common_type_t<First, Second, Rest...>;

	outtype out = std::forward<outtype>(aml::max(std::forward<First>(first), std::forward<Second>(second)));

	([&] {
		if (rest > out) out = std::forward<Rest>(rest);
	}(), ...);

	return aml::selectable_convert<OutType>(std::forward<outtype>(out));
}

/**
	@brief The sum of the input variadic arguments
*/
template<class OutType = aml::selectable_unused, class First, class... Rest> [[nodiscard]] constexpr
decltype(auto) sum_of(const First& first, const Rest&... rest) noexcept {
	decltype(first + (rest + ...)) out = first;
	([&] {
		out += rest;
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

/**
	@brief Absolute value or modulus of a number. @f$ |x| @f$
*/
template<class T> [[nodiscard]] constexpr
decltype(auto) abs(const T& val) noexcept {
	if constexpr (std::is_unsigned_v<T>) {
		return val;
	} else {
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

		const auto m = aml::max<common>(static_cast<common>(1), static_cast<common>(aml::abs(left)), static_cast<common>(aml::abs(right)));
		return (aml::abs(left - right)) <= (std::numeric_limits<common>::epsilon() * m);
	}
	else if constexpr (std::is_arithmetic_v<Left> && std::is_arithmetic_v<Right>) {
		using common = std::common_type_t<Left, Right>;
		return (static_cast<common>(left) == static_cast<common>(right));
	}
	else if constexpr (std::is_invocable_r_v<bool, std::equal_to<>, Left, Right>) {
		return (left == right);
	}
	else if constexpr (std::is_constructible_v<Left, decltype(right)> 
					   && std::is_invocable_r_v<bool, std::equal_to<>, Left, Left>) {
		return (left == static_cast<Left>(right));
	}
	else if constexpr (std::is_constructible_v<Right, decltype(left)>
					   && std::is_invocable_r_v<bool, std::equal_to<>, Right, Right>) {
		return (static_cast<Right>(left) == right);
	}
	else if constexpr (std::is_invocable_r_v<bool, std::equal_to<>, decltype(aml::unwrap(std::declval<Left>())), decltype(aml::unwrap(std::declval<Right>()))>) {
		return (aml::unwrap(left) == aml::unwrap(right));
	}
	else {
		static_assert(!sizeof(Left*), "Left and Right types do not have a equal operator");
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
}

template<class T> constexpr
const detail::equal_op_proxy<T> operator<(const T& left, const equal_fn&) { return { left }; }

template<class Left, class Right> constexpr
decltype(auto) operator>(const detail::equal_op_proxy<Left>& left, const Right& right) {
	return aml::equal(left.v, right);
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
		static_assert(!sizeof(T*), "Flooring not supported type");
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
		static_assert(!sizeof(T*), "Ceiling not supported type");
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
		static_assert(!sizeof(T*), "Rounding not supported type");
	}
}

}
