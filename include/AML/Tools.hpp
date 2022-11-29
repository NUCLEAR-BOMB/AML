#pragma once

#include <AML/_AMLCore.hpp>

#include <type_traits>

AML_NAMESPACE

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
				static_assert(!sizeof(Function*), "Functions/lambda must not return the value");
			}
			else {
				std::forward<Function>(fun)();
			}
		} else 
		{
			using t = decltype(std::forward<Function>(fun)(std::forward<FunVal>(val)));
			if constexpr (!std::is_void_v<t>) {
				static_assert(!sizeof(Function*), "Functions/lambda must not return the value");
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


AML_NAMESPACE_END
