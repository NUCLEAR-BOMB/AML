#pragma once

#include <AML/Functions.hpp>
#include <cstring>

namespace aml {

namespace algorithms
{
	template<bool use_max_precision, class T, std::enable_if_t<!use_max_precision, int> = 0> [[nodiscard]] constexpr
	auto newton_sqrt(const T& val, const T& start_val = 1, std::size_t iter_count = 5) noexcept
	{
		using outtype = std::common_type_t<float, T>;
			
		outtype x = static_cast<outtype>(start_val);
		for (; iter_count > 0; --iter_count) {
			x = outtype(0.5) * (x + (val / x));
		}
		return x;
	}

	template<bool use_max_precision, class T, std::enable_if_t<use_max_precision, int> = 0> [[nodiscard]] constexpr
	auto newton_sqrt(const T& val, const T& start_val = 1) noexcept
	{
		using outtype = std::common_type_t<float, T>;
		outtype x = static_cast<outtype>(start_val);

		auto lastx = x;
		while (true) {
			x = outtype(0.5) * (x + (static_cast<outtype>(val) / x));
			if (aml::equal(x, lastx)) break;
			lastx = x;
		}
		return x;
	}

	template<class Left, class Right, std::enable_if_t<std::is_unsigned_v<Right>, int> = 0> [[nodiscard]] constexpr
	auto binary_pow(Left left, Right right) noexcept {
		Left out = 1;
		while (right != 0) 
		{
			if (aml::odd(right))
				out *= left;

			left *= left;
			right >>= 1;
		}
		return out;
	}


}

}
