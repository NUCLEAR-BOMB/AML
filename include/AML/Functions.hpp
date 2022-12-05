#pragma once

#include <AML/Tools.hpp>
#include <limits>

AML_NAMESPACE

template<class OutType = aml::selectable_unused, class First> [[nodiscard]] constexpr
auto max(First&& first) noexcept {
	return aml::selectable_convert<OutType>(std::forward<First>(first));
}

template<class OutType = aml::selectable_unused, class First, class Second> [[nodiscard]] constexpr
auto max(First&& first, Second&& second) noexcept {
	return aml::selectable_convert<OutType>((first > second) ? (std::forward<First>(first)) : (std::forward<Second>(second)));
}

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


template<class OutType = aml::selectable_unused, class T> [[nodiscard]] constexpr
auto sqr(const T& val) noexcept {
	return aml::selectable_convert<OutType>(val * val);
}

template<class OutType = aml::selectable_unused, class T> [[nodiscard]] constexpr
auto abs(T&& val) noexcept {
	if constexpr (std::is_unsigned_v<T>) {
		return aml::selectable_convert<OutType>(std::forward<T>(val));
	} else {
		return aml::selectable_convert<OutType>((val < static_cast<T>(0)) ? (-val) : (val));
	}
}

template<class Left, class Right> [[nodiscard]] constexpr
bool equal(const Left& left, const Right& right) noexcept {
	if constexpr (std::is_floating_point_v<Left> && std::is_floating_point_v<Right>) {
		using type = std::common_type_t<Left, Right>;

		const type m = aml::max<type>(static_cast<type>(1), aml::abs<type>(left), aml::abs<type>(right));
		return (aml::abs(left - right)) <= (std::numeric_limits<type>::epsilon() * m);
	} else {
		return (left == right);
	}
}

template<class T> [[nodiscard]] constexpr
bool is_zero(const T& val) noexcept {
	return aml::equal(val, static_cast<T>(0));
}

template<class OutType = aml::selectable_unused, class Left, class Right> [[nodiscard]] constexpr
auto dist(const Left& left, const Right& right) noexcept {
	return aml::abs<OutType>(left - right);
}

AML_NAMESPACE_END
