#pragma once

#include <AML/_AMLCore.hpp>

#include <type_traits>

AML_NAMESPACE

inline constexpr std::size_t dynamic_extent = static_cast<std::size_t>(-1);

template<class T>
inline constexpr bool is_custom = std::is_class_v<T> || std::is_union_v<T> || std::is_enum_v<T>;

struct zero_t {
	template<class T, std::enable_if_t<!aml::is_custom<T>, int> = 0>
	explicit constexpr operator T() const noexcept { return static_cast<T>(0); }
};

struct one_t {
	template<class T, std::enable_if_t<!aml::is_custom<T>, int> = 0>
	explicit constexpr operator T() const noexcept { return static_cast<T>(1); }
};

template<std::size_t Direction>
struct unit_t {
	static constexpr auto dir = Direction;
	template<class T, std::enable_if_t<!aml::is_custom<T>, int> = 0>
	explicit constexpr operator T() const noexcept { return static_cast<T>(1); }
};

inline constexpr zero_t zero{};
inline constexpr one_t one{};

template<std::size_t Dir>
unit_t<Dir> unit{};

using selectable_unused = void;

template<class Out, class In> [[nodiscard]] constexpr
auto selectable_convert(In&& val) noexcept
{
	if constexpr (std::is_same_v<Out, selectable_unused>) {
		return std::forward<In>(val);
	} else {
		return static_cast<Out>(val);
	}
}

template<class Out, class In>
using selectable_type = std::conditional_t<std::is_same_v<Out, selectable_unused>, In, Out>;

namespace detail {
	template<class, class = std::size_t>
	struct is_complete_impl : public std::false_type {};

	template<class T>
	struct is_complete_impl<T, decltype(sizeof(T))> : public std::true_type {};
}

template<class T>
inline constexpr bool is_complete = detail::is_complete_impl<T>::value;


inline constexpr std::size_t STATIC_FOR_MAX = 30;

namespace detail
{
	template<class FunVal, class Function> constexpr
	void static_for_impl_call_fun(FunVal&& val, Function&& fun) noexcept
	{
		if constexpr (std::is_invocable_v<Function>)
		{
			using t = decltype(std::forward<Function>(fun)());
			if constexpr (!std::is_void_v<t>) {
				static_assert(!sizeof(Function*), "Functions/lambda must return void");
			}
			else {
				std::forward<Function>(fun)();
			}
		} else 
		{
			using t = decltype(std::forward<Function>(fun)(std::forward<FunVal>(val)));
			if constexpr (!std::is_void_v<t>) {
				static_assert(!sizeof(Function*), "Functions/lambda must return void");
			}
			else {
				std::forward<Function>(fun)(std::forward<FunVal>(val));
			}
		}
	}

	template<auto From, decltype(From) To, class Function> constexpr
	void static_for_impl2(Function&& fun) noexcept
	{
		if constexpr (From < To) 
		{
			detail::static_for_impl_call_fun(std::integral_constant<decltype(From), From>{}, std::forward<Function>(fun));
			
			detail::static_for_impl2<From + 1, To>(std::forward<Function>(fun));
		}
	}

	template<auto From, decltype(From) To, class Function> constexpr
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

template<auto From, decltype(From) To, class Function> constexpr
void static_for(Function&& fun) noexcept
{
	detail::static_for_impl<From, To>(std::forward<Function>(fun));
}

template<auto To, class Function> constexpr
void static_for(Function&& fun) noexcept
{
	detail::static_for_impl<static_cast<decltype(To)>(0), To>(std::forward<Function>(fun));
}

template<class... Args, class Function> constexpr
void variadic_loop(Args&&... args, Function&& fun) noexcept {
	static constexpr bool is_return_void = (... && std::is_void_v<decltype(fun(args))>);
	if constexpr (is_return_void) {
		(std::forward<Function>(fun)(args)...);
	} else {
		static_assert(!sizeof(Function*), "Functions/lambda must return void");
	}
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


template<std::size_t Bytes>
using signed_from_bytes = typename detail::signed_from_bytes_impl<Bytes>::type;

template<std::size_t Bytes>
using unsigned_from_bytes = std::make_unsigned_t<signed_from_bytes<Bytes>>;

template<std::size_t Bits>
using signed_from_bits = signed_from_bytes<(Bits + (CHAR_BIT - 1)) / CHAR_BIT>;

template<std::size_t Bits>
using unsigned_from_bits = std::make_unsigned_t<signed_from_bits<Bits>>;


template<std::size_t Bytes>
using floating_point_from_bytes = typename detail::floating_point_from_bytes_impl<Bytes>::type;

template<std::size_t Bits>
using floating_point_from_bits = floating_point_from_bytes<(Bits + (CHAR_BIT - 1)) / CHAR_BIT>;


namespace detail
{
	template<typename From, typename To, typename = void>
	struct is_narrowing_conversion_impl : std::true_type {};

	template<typename From, typename To>
	struct is_narrowing_conversion_impl<From, To, std::void_t<decltype(To{ std::declval<From>() })>> : std::false_type {};
}

template<class From, class To>
inline constexpr bool is_narrowing_conversion = detail::is_narrowing_conversion_impl<From, To>::value;

AML_NAMESPACE_END
