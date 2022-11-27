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

AML_NAMESPACE_END
