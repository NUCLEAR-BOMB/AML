#pragma once

#include <AML/Functions.hpp>
#include <cstring>

AML_NAMESPACE

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
			x = outtype(0.5) * (x + (val / x));
			if (aml::equal(x, lastx)) break;
			lastx = x;
		}
		return x;
	}
}


AML_NAMESPACE_END
