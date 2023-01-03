/** @file */
#pragma once

#include <AML/Tools.hpp>
#include <limits>

/** @cond */ AML_NAMESPACE /** @endcond */

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
decltype(auto) max(First&& first, Second&& second, Rest&&... rest) noexcept 
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
template<class OutType = aml::selectable_unused, class T> [[nodiscard]] constexpr
decltype(auto) abs(T&& val) noexcept {
	if constexpr (std::is_unsigned_v<T>) {
		return aml::selectable_convert<OutType>(std::forward<T>(val));
	} else {
		return aml::selectable_convert<OutType>((val < static_cast<T>(0)) ? (-val) : (val));
	}
}

/**
	@brief Checks if the @p left and @p right are equal
	@details Can compare a floating point value

	@see #not_equal
*/
template<class Left, class Right> [[nodiscard]] constexpr
bool equal(const Left& left, const Right& right) noexcept 
{
	if constexpr (std::is_floating_point_v<Left> && std::is_floating_point_v<Right>) 
	{
		using type = std::common_type_t<Left, Right>;

		const type m = aml::max<type>(static_cast<type>(1), aml::abs<type>(left), aml::abs<type>(right));
		return (aml::abs(left - right)) <= (std::numeric_limits<type>::epsilon() * m);
	} 
	else {
		return (left == right);
	}
}

/**
	@brief Checks if the @p left and @p right are not equal

	@see #equal
*/
template<class Left, class Right> [[nodiscard]] constexpr
bool not_equal(const Left& left, const Right& right) noexcept {
	return !aml::equal(left, right);
}

/**
	@brief Checks if the @p val is zero
	@details Can also check types that casts to @ref #zero "aml::zero"
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
template<class T> [[nodiscard]] constexpr /** @cond */ AML_FORCEINLINE /** @endcond */
bool odd(const T& val) noexcept 
{
	static_assert(!std::is_floating_point_v<T>, "T must not be a floating point number");
	return static_cast<bool>((val % static_cast<T>(2)) != static_cast<T>(0));
}

/**
	@brief Checks if @p val is even
*/
template<class T> [[nodiscard]] constexpr /** @cond */ AML_FORCEINLINE /** @endcond */
bool even(const T& val) noexcept {
	return !aml::odd(val);
}

/**
	@brief Checks if @p val is negative
*/
template<class T> [[nodiscard]] constexpr /** @cond */ AML_FORCEINLINE /** @endcond */
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
template<class T> [[nodiscard]] constexpr /** @cond */ AML_FORCEINLINE /** @endcond */
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
		const auto t = static_cast<std::int_fast64_t>(val);
		using conv = aml::selectable_type<OutType, T>;
		return static_cast<conv>(t - (t > val ? 1 : 0));
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
		return static_cast<conv>(t + (t < val ? 1 : 0));
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



AML_NAMESPACE_END
