/** @file */
#pragma once

#include <AML/_AMLCore.hpp>

#include <type_traits>
#include <tuple>
#include <functional>
#include <cstddef>

#include <vector>
#include <list>
#include <deque>
#include <array>

#ifdef AML_LIBRARY
	#define AML_LIBRARY_TOOLS
#else
	#error AML library is required
#endif

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

#if !AML_CXX20
namespace detail {
	template<class T>
	struct type_identity_impl {
		using type = T;
	};
}
template<class T>
using type_identity = typename detail::template type_identity_impl<T>::type;
#else
template<class T>
using type_identity = typename std::template type_identity<T>::type;
#endif

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
	constexpr fill_initializer(T&& val) noexcept	  : value(std::move(val)) {}

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

	AML_CONSTEVAL 
	operator value_type() const noexcept { return value; }

	AML_CONSTEVAL 
	value_type operator()() const noexcept { return value; }
};

/// @endcond

inline constexpr zero_t zero{}; ///< Shortcut for zero_t{}
inline constexpr one_t one{};	///< Shortcut for one_t{}

template<std::size_t Dir>
inline constexpr unit_t<Dir> unit{}; ///< Shortcut for unit_t<Dir>{}

template<auto V>
inline constexpr auto constant = constant_t<V>{}; ///< Shortcut for constant_t<V>{}


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
} // namespace detail

/**
	@brief Checks if @p T is not forward declarated
*/
template<class T>
inline constexpr bool is_complete = detail::is_complete_impl<T>::value;

/**
	@brief Max recursion level for #aml::static_for
	@details If the value is greater, then runtime for loop is used
*/
inline constexpr std::size_t STATIC_FOR_MAX = std::numeric_limits<std::size_t>::max();

//inline constexpr std::size_t STATIC_FOR_MAX = 30;

namespace detail
{
	template<class FunVal, class Function> constexpr
	void static_for_impl_call_fun(FunVal&& val, Function&& fun) noexcept
	{
		if constexpr (std::is_invocable_v<Function>) // checks if function hasn't any arguments
		{
			if constexpr (!std::is_void_v<decltype(std::invoke(fun))>) {
				static_assert(!sizeof(Function*), "Functions/lambda must return void");
			} else {
				std::invoke(fun);
			}
		} else {
			if constexpr (!std::is_void_v<decltype(std::invoke(fun, std::forward<FunVal>(val)))>) {
				static_assert(!sizeof(Function*), "Functions/lambda must return void");
			} else {
				std::invoke(fun, std::forward<FunVal>(val));
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
} // namespace detail

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

template<std::size_t I, class T> constexpr
decltype(auto) get(T&& val) noexcept(noexcept(std::forward<T>(val).get<I>())) {
	return std::forward<T>(val).get<I>();
}

namespace detail
{
	template<class, template<class...> class>
	struct is_specialization_impl : std::false_type {};

	template<template <class...> class Template, class... Args>
	struct is_specialization_impl<Template<Args...>, Template> : std::true_type {};
}

template<class T, template <class...> class TP>
inline constexpr bool is_specialization_of = detail::template is_specialization_impl<T, TP>::value;

namespace detail
{
	template<template<class...> class, template<class...> class>
	struct is_template_same_impl : std::false_type {};

	template<template<class...> class TT>
	struct is_template_same_impl<TT, TT> : std::true_type {};
}

template<template<class...> class First, template<class...> class Second>
inline constexpr bool is_template_same = detail::template is_template_same_impl<First, Second>::value;

namespace detail
{
	template<class First, class Second>
	struct is_same_specialization_impl;

	template<template<class...> class First, template<class...> class Second, class... FirstParams, class... SecondParams>
	struct is_same_specialization_impl<First<FirstParams...>, Second<SecondParams...>>
	{
		static constexpr bool value = aml::is_template_same<First, Second>;
	};
}

template<class First, class Second>
inline constexpr bool is_same_specialization = detail::template is_same_specialization_impl<First, Second>::value;

namespace detail
{
	template<class T, class = void>
	struct unwrap_has_value_member : std::false_type {};
	template<class T>
	struct unwrap_has_value_member<T, std::void_t<decltype(T::value, 0)>> : std::true_type {};

	template<class T, class = void>
	struct unwrap_has_get_method : std::false_type {};
	template<class T>
	struct unwrap_has_get_method<T, std::void_t<decltype(std::declval<T>().get(), 0)>> : std::true_type {};

	template<class T, class = void>
	struct unwrap_has_v_member : std::false_type {};
	template<class T>
	struct unwrap_has_v_member<T, std::void_t<decltype(T::v, 0)>> : std::true_type {};
}

template<class T> constexpr
decltype(auto) unwrap(T&& val) noexcept 
{
	using type = aml::remove_cvref<T>;

	if constexpr (detail::template unwrap_has_value_member<type>{}) {
		return std::forward<T>(val).value;
	} else if constexpr (detail::template unwrap_has_get_method<type>{}) {
		return std::forward<T>(val).get();
	} else if constexpr (detail::template unwrap_has_v_member<type>{}) {
		return std::forward<T>(val).v;
	} else {
		return std::forward<T>(val);
	}
}

namespace detail
{
	template<std::size_t Pos, class First, class... Rest>
	struct get_variadic_impl {
		static decltype(auto) get(First&&, Rest&&... args) {
			return get_variadic_impl<Pos - 1, Rest...>::get(std::forward<Rest>(args)...);
		}
	};

	template<class First, class... Rest>
	struct get_variadic_impl<0, First, Rest...> {
		static decltype(auto) get(First&& first, Rest&&...) {
			return std::forward<First>(first);
		}
	};
}

template<std::size_t Pos, class... Args> constexpr
decltype(auto) get_variadic(Args&&... args) noexcept {
	return detail::template get_variadic_impl<Pos, Args...>::get(std::forward<Args>(args)...);
}

constexpr bool is_digit(unsigned char chr) noexcept {
	return (chr >= '0' && '9' >= chr);
}

constexpr bool is_graph(unsigned char chr) noexcept {
	return (chr >= '!' && '~' >= chr);
}

constexpr bool is_space(unsigned char chr) noexcept {
	return (chr >= '\t' && '\r' >= chr) || (chr == ' ');
}

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

	template<std::size_t Bytes, bool = std::is_void_v<typename signed_from_bytes_impl<Bytes>::type>>
	struct signed_from_bytes_impl2;

	template<std::size_t Bytes>
	struct signed_from_bytes_impl2<Bytes, false> {
		using type = typename signed_from_bytes_impl<Bytes>::type;
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

	template<std::size_t Bytes, bool = std::is_void_v<typename floating_point_from_bytes_impl<Bytes>::type>>
	struct floating_point_from_bytes_impl2;

	template<std::size_t Bytes>
	struct floating_point_from_bytes_impl2<Bytes, false> {
		using type = typename floating_point_from_bytes_impl<Bytes>::type;
	};

} // namespace detail

/**
	@brief Converts a number of @b bytes to a type whose size is no larger than the size of the signed integral
	@details 
			If the number of @b bytes is greater than the maximum signed integral type, it will be @c void type. @n@n

			@b Example: @n
			0..1	&rArr; @c int8  @n
			2..2	&rArr; @c int16 @n
			3..4	&rArr; @c int32 @n
			5..8	&rArr; @c int64 @n
			>8		&rArr; @c error
*/
template<std::size_t Bytes>
using signed_from_bytes = typename detail::template signed_from_bytes_impl2<Bytes>::type;

/**
	@brief Converts a number of @b bytes to a type whose size is no larger than the size of the unsigned integral
	@details
			If the number of @b bytes is greater than the maximum unsigned integral type, it will be @c void type. @n@n

			@b Example: @n
			0..1	&rArr; @c uint8  @n
			2..2	&rArr; @c uint16 @n
			3..4	&rArr; @c uint32 @n
			5..8	&rArr; @c uint64 @n
			>8		&rArr; @c error
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
			>64		&rArr; @c error

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
			>64		&rArr; @c error

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
using floating_point_from_bytes = typename detail::template floating_point_from_bytes_impl2<Bytes>::type;

/**
	@brief Converts a number of @b bits to a type whose size is no larger than the size of the floating point
	@details
			If the number of @b bits is greater than the maximum floating point type, it will be @c void type

	@see floating_point_from_bytes
*/
template<std::size_t Bits>
using floating_point_from_bits = floating_point_from_bytes<(Bits + (CHAR_BIT - 1)) / CHAR_BIT>;


template<class... Ts>
struct common_type_body 
{
	using type = std::common_type_t<Ts...>;
};

template<class... Ts>
using common_type = typename common_type_body<Ts...>::type;

namespace detail
{
	template<typename From, typename To, typename = void>
	struct is_narrowing_conversion_impl 
		: std::true_type {};

	template<typename From, typename To>
	struct is_narrowing_conversion_impl<From, To, std::void_t<decltype(To{ std::declval<From>() })>> 
		: std::false_type {};
} // namespace detail

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

namespace detail
{
	template<class Container, class U, class = void>
	struct has_rebind_impl
		: std::false_type {};

	template<class Container, class U>
	struct has_rebind_impl<Container, U, std::void_t<typename Container::template rebind<U>::type>>
		: std::true_type {};
} // namespace detail

/**
	@brief Checks if @p Container has <tt>rebind<U>::type</tt>
*/
template<class Container, class U = int>
inline constexpr bool has_rebind = detail::template has_rebind_impl<Container, U>::value;

namespace detail
{
	template<class Container>
	struct container_parameters_impl;

	template<template <class, class...> class Container, class T, class... Parameters>
	struct container_parameters_impl<Container<T, Parameters...>>
	{
		using type = std::tuple<Parameters...>;
	};

	template<class Container>
	struct container_value_impl;

	template<template <class, class...> class Container, class T, class... Parameters>
	struct container_value_impl<Container<T, Parameters...>> 
	{
		using type = T;
	};
} // namespace detail

/**
	@brief Template type alias of @p Container parameters as a @c std::tuple<...>
*/
template<class Container>
using container_parameters = typename detail::template container_parameters_impl<Container>::type;

/**
	@brief Template type alias of @p Container value type
*/
template<class Container>
using container_value_type = typename detail::template container_value_impl<Container>::type;

namespace detail
{
	template<class Container, class U, bool = aml::has_rebind<Container, U>>
	struct rebind_container_impl;

	template<template <class, class...> class Container, class T, class... Parameters, class U>
	struct rebind_container_impl<Container<T, Parameters...>, U, false>
	{
		// if hasn't Container::rebind<...>::type
		using type = Container<U, Parameters...>;
	};

	template<class Container, class U>
	struct rebind_container_impl<Container, U, true>
	{
		// if has Container::rebind<...>::type
		using type = typename Container::template rebind<U>::type;
	};
}

/**
	@brief Struct that rebinds container value type
	@details You can add new specializations to this struct to add support for a container

	@see aml::rebind_container
*/
template<class Container, class U>
struct rebind_container_body
{
	using type = typename detail::template rebind_container_impl<Container, U>::type;
};

/**
	@brief Rebinds @p Container as a container with value type of @p U

	@see aml::rebind_container_body
*/
template<class Container, class U>
using rebind_container = typename aml::template rebind_container_body<Container, U>::type;

template<class T, class Allocator, class U>
struct rebind_container_body<std::vector<T, Allocator>, U>
{
	using type = std::vector<U, rebind_container<Allocator, U>>;
};

template<class T, class Allocator, class U>
struct rebind_container_body<std::deque<T, Allocator>, U>
{
	using type = std::deque<U, rebind_container<Allocator, U>>;
};

template<class T, class Allocator, class U>
struct rebind_container_body<std::list<T, Allocator>, U>
{
	using type = std::list<U, rebind_container<Allocator, U>>;
};

/**
	@brief Verifies if @p Left nad @p Right have same parameters
	@details Creates compile-time if it does not pass assert
*/
template<class LeftContainer, class RightContainer>
constexpr void verify_container_parameters() noexcept {
	using lp = aml::container_parameters<LeftContainer>;
	using rp = aml::container_parameters<RightContainer>;

	static_assert(std::is_same_v<lp, rp>, "The parameters of the containers must be the same");
}

namespace detail 
{
	// meta if else
	template<class T, class = void>
	struct get_value_type_impl {
		// 1: else
		template<class T, class = void>
		struct nested_1 {
			// 2: else
			template<class T, bool = std::is_same_v<std::remove_reference_t<decltype(aml::unwrap(std::declval<T>()))>, T>>
			struct nested_2 {
				// 3: if
				using type = std::remove_reference_t<decltype(aml::unwrap(std::declval<T>()))>;
			};
			template<template <class, class...> class Container, class T, class... Params>
			struct nested_2<Container<T, Params...>, true> {
				// 3: else
				using type = T;
			};
			using type = typename nested_2<T>::type;
		};
		template<class T>
		struct nested_1<T, std::void_t<typename T::type>> {
			// 2: if
			using type = typename T::type;
		};
		using type = typename nested_1<T>::type;
	};

	template<class T>
	struct get_value_type_impl<T, std::void_t<typename T::value_type>> {
		// 1: if
		using type = typename T::value_type;
	};

} // namespace detail

/**
	@brief Get value type of container
*/
template<class T>
using value_type_of = typename detail::template get_value_type_impl<aml::remove_cvref<T>>::type;

//template<class T>
//using remove_ptr_and_ref = std::remove_reference_t<std::remove_pointer_t<std::decay_t<T>>>;

//template<class T, class From>
//inline constexpr bool is_immutable_ref = std::is_constructible_v<T, const From&> || 
//										 std::is_assignable_v<T, const From&>;

//template<class T>
//inline constexpr bool is_const_ref = std::is_const_v<std::remove_reference_t<T>>;

namespace detail
{
	template<class R, class... Args>
	struct function_traits_impl_base 
	{
		using result_type = R;

		using args_as_tuple = std::tuple<Args...>;

		template<std::size_t I>
		using arg = std::tuple_element_t<I, args_as_tuple>;
	};

	template<class>
	struct function_traits_impl;

	template<class R, class... Args> struct function_traits_impl<R(Args..., ...)> 
		: function_traits_impl_base<R, Args...> {};

	template<class R, class... Args> struct function_traits_impl<std::function<R(Args...)>> 
		: function_traits_impl_base<R, Args...> {};

	template<class R, class... Args> struct function_traits_impl<R(Args...)> 
		: function_traits_impl_base<R, Args...> {};
}

template<class Func>
using function_traits = detail::template function_traits_impl<Func>;


template<class T, std::size_t ReserveSize>
class Pushable_array : private std::array<T, ReserveSize>
{
	using Base = std::array<T, ReserveSize>;
public:

	static constexpr auto static_reserved_size = ReserveSize;

	using container_type = std::array<T, ReserveSize>;

	using Base::value_type;
	using Base::size_type;

	using Base::reference;
	using Base::const_reference;

	using Base::iterator;
	using Base::const_iterator;
	using Base::reverse_iterator;
	using Base::const_reverse_iterator;

	using Base::front;
	using Base::data;

	using Base::begin;
	using Base::cbegin;
	using Base::rend;
	using Base::crend;
	
	using Base::fill;
	using Base::swap;

	constexpr
	Pushable_array() noexcept
		: m_current_size(0) 
		, Base{} {}

	[[nodiscard]] constexpr 
	size_type size() const noexcept { return m_current_size; }

	[[nodiscard]] constexpr
	bool empty() const noexcept {
		return (m_current_size == 0);
	}

	constexpr iterator end() noexcept { return this->Base::begin() + this->size(); }
	constexpr const_iterator end() const noexcept { return this->Base::begin() + this->size(); }
	constexpr const_iterator cend() const noexcept { return this->end(); }

	constexpr reverse_iterator rbegin() noexcept { return this->Base::rend() - this->size(); }
	constexpr const_reverse_iterator rbegin() const noexcept { return this->Base::rend() - this->size(); }
	constexpr const_reverse_iterator crbegin() const noexcept { return this->rbegin(); }

	constexpr reference back() noexcept {
		return (*this)[m_current_size - 1];
	}
	constexpr const_reference back() const noexcept {
		return (*this)[m_current_size - 1];
	}

	[[nodiscard]] static AML_CONSTEVAL
	size_type reserved_size() noexcept {
		return ReserveSize;
	}

	constexpr
	void push_back(const value_type& value) noexcept 
	{
		AML_DEBUG_VERIFY(m_current_size < reserved_size(), "Maximum limit is used");
		this->Base::operator[](m_current_size) = value;
		++m_current_size;
	}

	constexpr
	value_type pop_back() const noexcept {
		--m_current_size;
		return this->Base::operator[](m_current_size);
	}

	constexpr
	void resize(size_type new_size) noexcept {
		AML_DEBUG_VERIFY(new_size < reserved_size(), "Maximum limit is used");
		m_current_size = new_size;
	}

	constexpr
	reference operator[](size_type index) noexcept {
		AML_DEBUG_VERIFY(index < size(), "Out of range");
		return this->Base::operator[](index);
	}
	constexpr
	const_reference operator[](size_type index) const noexcept {
		return const_cast<Pushable_array&>(*this)[index];
	}


private:
	mutable size_type m_current_size;
};

}
