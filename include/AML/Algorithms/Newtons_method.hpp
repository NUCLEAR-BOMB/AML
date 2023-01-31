#pragma once
#include <AML/Functions.hpp>

namespace aml
{
namespace algorithms
{

namespace detail
{
	template<class T, class Function, class CompFunction> constexpr
	auto raw_newtons_method_impl(const T& start_val, Function&& func, CompFunction&& comp) noexcept
	{
		using return_type = aml::common_type<T, float>;
		return_type iter_val = static_cast<return_type>(start_val);
		auto last_val = iter_val;
		while (true) 
		{
			iter_val = static_cast<return_type>(func(std::as_const(iter_val)));

			AML_DEBUG_VERIFY(!(std::isnan(iter_val) || std::isinf(iter_val)), "Iteration value is NaN or infinity");

			if (comp(iter_val, last_val)) break;

			last_val = iter_val;
		}
		return iter_val;
	}

}

template<class T, class Function> [[nodiscard]] constexpr
auto raw_newtons_method(
	Function&& func, 
	const T& start_val = static_cast<T>(aml::zero)
) noexcept
{
	return detail::raw_newtons_method_impl<T>(start_val, std::forward<Function>(func), [](const auto& iter_val, const auto& last_val) {
		return aml::equal(iter_val, last_val);
	});
}

template<class T, class Function> [[nodiscard]] constexpr
auto raw_strong_newtons_method(Function&& func, const T& start_val = static_cast<T>(aml::zero))
{
	std::size_t hit_count = 0;
	return detail::raw_newtons_method_impl<T>(start_val, std::forward<Function>(func), [&hit_count](const auto& iter_val, const auto& last_val) {

		bool is_equal = aml::equal(iter_val, last_val);
		hit_count = is_equal ? ++hit_count : 0;

		return (hit_count > 5);
	});
}

template<class T, class Function> [[nodiscard]] constexpr
auto raw_newtons_method_with_max_iteration(
	Function&& func,
	std::size_t max_iter,
	const T& start_val = static_cast<T>(aml::zero)
) noexcept
{
	return detail::raw_newtons_method_impl<T>(start_val, std::forward<Function>(func), [&max_iter](const auto& iter_val, const auto& last_val) {
		return ((--max_iter) == 0 || aml::equal(iter_val, last_val));
	});
}

template<class T, class Function> [[nodiscard]] constexpr
auto raw_newtons_method_with_min_accuracy(
	Function&& func,
	const T& accuracy,
	const T& start_val = static_cast<T>(aml::zero)
) noexcept
{
	return detail::raw_newtons_method_impl<T>(start_val, std::forward<Function>(func), [&accuracy](const auto& iter_val, const auto& last_val) {
		return (aml::abs(iter_val - last_val) < accuracy);
	});
}

}

}