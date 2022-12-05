#pragma once

#include <AML/Functions.hpp>
#include <cstring>

AML_NAMESPACE

namespace algorithms
{
	namespace detail
	{
		template<class T> [[nodiscard]] constexpr
		auto newton_sqrt_impl(const T& val, const T& start_val, std::size_t iter_count) noexcept
		{
			//static_assert(std::is_floating_point_v<OutType>, "OutType must be a floating point type");

			// f(x) = x^2 - val
			// f'(x) = 2x

			using outtype = std::common_type_t<float, T>;

			//if (val < outtype(10000))		x = outtype(44.72135955)  + outtype(0.011180339887)   * (val - outtype(2000));
			//else if (val < outtype(200000)) x = outtype(1000)		  + outtype(0.0005)			  * (val - outtype(1000000));
			//else	x = outtype(2236.0679775) + outtype(0.00022360679775) * (val - outtype(5000000));

			outtype x = static_cast<outtype>(start_val);
			if (iter_count > 0) {
				for (; iter_count > 0; --iter_count)
				{
					x = outtype(0.5) * (x + (val / x));
				}
			}
			else {
				auto lastx = x;
				while (true)
				{
					x = outtype(0.5) * (x + (val / x));
					if (aml::equal(x, lastx)) break;
					lastx = x;
				}
			}

			return x;
		}
	}

	template<class T> [[nodiscard]] constexpr
	auto newton_sqrt(const T& val, std::size_t iter_count = 0) noexcept {
		const auto start_val = (val / static_cast<T>(10));
		if constexpr (std::is_integral_v<T>) {
			return detail::newton_sqrt_impl(val, (start_val == 0 ? static_cast<T>(1) : start_val), iter_count);
		} else {
			return detail::newton_sqrt_impl(val, start_val, iter_count);
		}
	}

	template<class T> [[nodiscard]] constexpr
	auto newton_sqrt(const T& val, const T& start_val, std::size_t iter_count = 0) noexcept {
		return detail::newton_sqrt_impl(val, start_val, iter_count);
	}

	
}


AML_NAMESPACE_END
