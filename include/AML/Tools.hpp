/** @file */
#pragma once

#include <AML/_AMLCore.hpp>

#include <type_traits>
#include <tuple>
#include <functional>
#include <cstddef>

namespace aml {

inline constexpr std::size_t dynamic_extent = static_cast<std::size_t>(-1); ///< std::dynamic_extent clone

#if !AML_CXX20
template<class T>
using remove_cvref = std::remove_cv_t<typename std::template remove_reference_t<T>>; ///< C++20 std::remove_cvref clone
#else // AML_CXX20
template<class T>
using remove_cvref = std::remove_cvref_t<T>;
#endif // AML_CXX20

template<class T>
inline constexpr bool is_custom = std::is_class_v<T> || std::is_union_v<T> || std::is_enum_v<T>; ///< Checks if @c T is user created


/// Wrapper around @c std::size_t
struct size_initializer
{
	using size_type = std::size_t;

	constexpr size_initializer(const size_type size_) noexcept : size(size_) {}
	
	constexpr operator size_type() const noexcept { return this->size; }

	const size_type size;
};

/// Wrapper around @c T
template<class T>
struct fill_initializer
{
	using value_type = T;

	constexpr fill_initializer(const T& val) noexcept : value(val) {}
	constexpr fill_initializer(T&& val) noexcept : value(std::move(val)) {}

	const value_type value;
};

/// @cond

/// Type of @ref zero
struct zero_t {
	template<class T>
	explicit constexpr operator T() const noexcept { return static_cast<T>(0); }
};

/// Type of @ref one
struct one_t {
	template<class T>
	explicit constexpr operator T() const noexcept { return static_cast<T>(1); }
};

/// Type of @ref unit
template<std::size_t Direction>
struct unit_t {
	static constexpr auto dir = Direction;
	template<class T>
	explicit constexpr operator T() const noexcept { return static_cast<T>(1); }
};

/// Clone of std::integral_constant ?
template<auto V>
struct constant_t {
	static constexpr auto value = V;
	using value_type = decltype(V);
	using type = constant_t<V>;

	template<class T>
	constexpr operator T() const noexcept { return V; }

	constexpr operator std::integral_constant<value_type, value>() const noexcept {
		return std::integral_constant<value_type, value>{};
	}

	constexpr value_type operator()() const noexcept { return V; }
};

/// @endcond

inline constexpr zero_t zero{}; ///< Shortcut for zero_t{}
inline constexpr one_t one{};	///< Shortcut for one_t{}

template<std::size_t Dir>
unit_t<Dir> unit{}; ///< Shortcut for unit_t<Dir>{}

template<auto V>
static constexpr auto constant = constant_t<V>{}; ///< Shortcut for constant_t<V>{}


using selectable_unused = void; ///< Used in @ref selectable_convert to ignore cast

/**
	@brief Used to convert @p In to <tt>Out</tt> if Out isn't #aml::selectable_unused type

	@see selectable_type
*/
template<class Out, class In> [[nodiscard]] constexpr
std::enable_if_t<std::is_same_v<Out, selectable_unused>, In&&> selectable_convert(In&& val) noexcept {
	return std::forward<In>(val);
}

template<class Out, class In> [[nodiscard]] constexpr
std::enable_if_t<!std::is_same_v<Out, selectable_unused>, Out> selectable_convert(const In& val) noexcept {
	return static_cast<Out>(val);
}



/**
	@brief Template type alias that selects type.
	@details If @p Out isn't #aml::selectable_unused then the type alias is <tt>In</tt>, otherwise it will be Out

	@see #aml::selectable_convert
*/
template<class Out, class In>
using selectable_type = std::conditional_t<std::is_same_v<Out, selectable_unused>, In, Out>;

namespace detail {
	template<class, class = std::size_t>
	struct is_complete_impl : public std::false_type {};

	template<class T>
	struct is_complete_impl<T, decltype(sizeof(T))> : public std::true_type {};
}

/**
	@brief Checks if @p T is not forward declarated
*/
template<class T>
inline constexpr bool is_complete = detail::is_complete_impl<T>::value;

/**
	@brief Max recursion level for #aml::static_for
	@details If the value is greater, then runtime for loop is used
*/
inline constexpr std::size_t STATIC_FOR_MAX = 30;

namespace detail
{
	template<class FunVal, class Function> constexpr
	void static_for_impl_call_fun(FunVal&& val, Function&& fun) noexcept
	{
		if constexpr (std::is_invocable_v<Function>) // checks if function hasn't any arguments
		{
			using t = decltype(std::invoke(fun));
			if constexpr (!std::is_void_v<t>) {
				static_assert(!sizeof(Function*), "Functions/lambda must return void");
			}
			else {
				std::invoke(fun);
			}
		} else 
		{
			using t = decltype(std::invoke(fun, val));
			if constexpr (!std::is_void_v<t>) {
				static_assert(!sizeof(Function*), "Functions/lambda must return void");
			}
			else {
				std::invoke(fun, val);
			}
		}
	}

	template<auto From, auto To, class Function> constexpr
	void static_for_impl2(Function&& fun) noexcept
	{
		if constexpr (From < To) 
		{
			detail::static_for_impl_call_fun(std::integral_constant<decltype(From + To), From>{}, std::forward<Function>(fun));
			
			detail::static_for_impl2<From + 1, To>(std::forward<Function>(fun));
		}
	}

	template<auto From, auto To, class Function> constexpr
	void static_for_impl(Function&& fun) noexcept
	{
		if constexpr (static_cast<std::size_t>(To - From) <= STATIC_FOR_MAX) {
			detail::static_for_impl2<From, To>(std::forward<Function>(fun));
		} else {
			for (auto i = From; i < To; ++i)
			{
				detail::static_for_impl_call_fun(i, std::forward<Function>(fun));
			}
		}
	}
}

/**
	@brief Compile-time for loop
	@details Calls @p Function @p To - @p From + 1 once or and pass [From, To] (integer closed interval) values

	@tparam From Begin of interval
	@tparam To End of interval
	@tparam Function Function type of called function

	@param fun Function, that will be called
*/
template<auto From, auto To, class Function> constexpr
void static_for(Function&& fun) noexcept
{
	detail::static_for_impl<From, To>(std::forward<Function>(fun));
}

/**
	@brief @copybrief aml::static_for
	@details Calls @p Function @p To + 1 once or and pass [0, To] (integer closed interval) values

	@tparam To End of interval
	@tparam Function Function type of called function

	@param fun Function, that will be called
*/
template<auto To, class Function> constexpr
void static_for(Function&& fun) noexcept
{
	detail::static_for_impl<static_cast<decltype(To)>(0), To>(std::forward<Function>(fun));
}

#if 0
template<class... Args, class Function> constexpr
void variadic_loop(Args&&... args, Function&& fun) noexcept {
	static constexpr bool is_return_void = (... && std::is_void_v<decltype(fun(args))>);
	if constexpr (is_return_void) {
		(std::forward<Function>(fun)(args)...);
	} else {
		static_assert(!sizeof(Function*), "Functions/lambda must return void");
	}
}
#endif

namespace detail
{
	template<std::size_t Bytes>
	struct signed_from_bytes_impl
	{
		using type = 
			std::conditional_t<Bytes <= 1, std::int8_t,
			std::conditional_t<Bytes <= 2, std::int16_t,
			std::conditional_t<Bytes <= 4, std::int32_t,
			std::conditional_t<Bytes <= 8, std::int64_t,
			void
		>>>>;
	};

	template<std::size_t Bytes>
	struct floating_point_from_bytes_impl
	{
		using type =
			std::conditional_t<Bytes <= sizeof(float), float,
			std::conditional_t<Bytes <= sizeof(double), double,
			std::conditional_t<(sizeof(long double) > sizeof(double)), long double,
			void
		>>>;
	};
}

/**
	@brief Converts a number of @b bytes to a type whose size is no larger than the size of the signed integral
	@details 
			If the number of @b bytes is greater than the maximum signed integral type, it will be @c void type. @n@n

			@b Example: @n
			0..1	&rArr; @c int8  @n
			2..2	&rArr; @c int16 @n
			3..4	&rArr; @c int32 @n
			5..8	&rArr; @c int64 @n
			>8		&rArr; @c void
*/
template<std::size_t Bytes>
using signed_from_bytes = typename detail::template signed_from_bytes_impl<Bytes>::type;

/**
	@brief Converts a number of @b bytes to a type whose size is no larger than the size of the unsigned integral
	@details
			If the number of @b bytes is greater than the maximum unsigned integral type, it will be @c void type. @n@n

			@b Example: @n
			0..1	&rArr; @c uint8  @n
			2..2	&rArr; @c uint16 @n
			3..4	&rArr; @c uint32 @n
			5..8	&rArr; @c uint64 @n
			>8		&rArr; @c void
*/
template<std::size_t Bytes>
using unsigned_from_bytes = std::make_unsigned_t<signed_from_bytes<Bytes>>;

/**
	@brief Converts a number of @b bits to a type whose size is no larger than the size of the signed integral
	@details
			If the number of @b bits is greater than the maximum signed integral type, it will be @c void type. @n@n

			@b Example: @n
			0..8	&rArr; @c int8  @n
			9..16	&rArr; @c int16 @n
			17..32	&rArr; @c int32 @n
			33..64	&rArr; @c int64 @n
			>64		&rArr; @c void

	@see signed_from_bytes
*/
template<std::size_t Bits>
using signed_from_bits = signed_from_bytes<(Bits + (CHAR_BIT - 1)) / CHAR_BIT>;

/**
	@brief Converts a number of @b bits to a type whose size is no larger than the size of the unsigned integral
	@details
			If the number of @b bits is greater than the maximum unsigned integral type, it will be @c void type. @n@n

			@b Example: @n
			0..8	&rArr; @c uint8 @n
			9..16	&rArr; @c uint16 @n
			17..32	&rArr; @c uint32 @n
			33..64	&rArr; @c uint64 @n
			>64		&rArr; @c void

	@see unsigned_from_bytes
*/
template<std::size_t Bits>
using unsigned_from_bits = std::make_unsigned_t<signed_from_bits<Bits>>;

/**
	@brief Converts a number of @b bytes to a type whose size is no larger than the size of the floating point
	@details 
			If the number of @b bytes is greater than the maximum floating point type, it will be @c void type

	@see signed_from_bytes @n
		 unsigned_from_bytes
*/
template<std::size_t Bytes>
using floating_point_from_bytes = typename detail::template floating_point_from_bytes_impl<Bytes>::type;

/**
	@brief Converts a number of @b bits to a type whose size is no larger than the size of the floating point
	@details
			If the number of @b bits is greater than the maximum floating point type, it will be @c void type

	@see floating_point_from_bytes
*/
template<std::size_t Bits>
using floating_point_from_bits = floating_point_from_bytes<(Bits + (CHAR_BIT - 1)) / CHAR_BIT>;


namespace detail
{
	template<typename From, typename To, typename = void>
	struct is_narrowing_conversion_impl 
		: std::true_type {};

	template<typename From, typename To>
	struct is_narrowing_conversion_impl<From, To, std::void_t<decltype(To{ std::declval<From>() })>> 
		: std::false_type {};
}

/**
	@brief Checks if the @p From type is has a narrowing conversion to the @p To type
	@details
			@b Example: @n
			From,	To		= result @n@n
			
			int8,	int16	= false @n
			uint8,	int16	= false @n
			float,	double	= false @n
			double, float	= true  @n
			int64,	int32	= true
*/
template<class From, class To>
inline constexpr bool is_narrowing_conversion = detail::template is_narrowing_conversion_impl<From, To>::value;


template<class Container>
struct get_container_data;

/// Contains information about the container
template<template <class, class...> class Container, class T, class... Parameters>
struct get_container_data<Container<T, Parameters...>>
{
	using type = Container<T, Parameters...>; ///< Container itself

	template<class U>
	using create = Container<U, Parameters...>; ///< Creating a new container with a replaced value type

	using value_type = T; ///< Container value type
	using parameters = std::tuple<Parameters...>; ///< Container parameters
};

namespace detail
{
	template<class T, class = void>
	struct has_container_structure_impl : std::false_type {};

	template<class T>
	struct has_container_structure_impl<T, std::void_t<typename get_container_data<T>::type>> : std::true_type {};
}

/// Checks if @p T has container structure
template<class T>
inline constexpr bool has_container_structure = detail::template has_container_structure_impl<T>::value;

/**
	@brief Verifies if @p Left nad @p Right have same parameters
	@details Creates compile-time if it does not pass assert

	@tparam Left Container data
	@tparam Right Container data

	@see get_container_data
*/
template<class Left, class Right>
constexpr void verify_container_parameters() noexcept {
	static_assert(std::is_same_v<typename Left::parameters, typename Right::parameters>,
		"The parameters of the containers must be the same");
}

namespace detail {

	template<class T, class = void>
	struct get_value_type_impl {
		static_assert(!sizeof(T*), "T must have a type alias \"value_type\"");
		using type = void;
	};

	template<class T>
	struct get_value_type_impl<T, std::void_t<typename std::template decay_t<T>::value_type>> {
		using type = typename std::template decay_t<T>::value_type;
	};
}

/**
	@brief Get value type of container
	@details If the type doesn't have an type alias of value_type - creates compile-time error
*/
template<class T>
using get_value_type = typename detail::template get_value_type_impl<T>::type;

}
