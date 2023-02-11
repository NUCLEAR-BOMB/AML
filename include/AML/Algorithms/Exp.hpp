#pragma once

#include <AML/Functions.hpp>

namespace aml
{
namespace algorithms
{

template<unsigned Steps = 0, class T> constexpr
auto taylor_exp(const T& val) noexcept
{
	using result_t = aml::common_type<T, float>;

	const auto fval = static_cast<result_t>(val);

	auto out = 1 + fval;
	auto next = fval;

	decltype(Steps) stepc = 2;

	while (true)
	{
		auto last = out;

		next *= (fval / stepc);
		out += next;

		if (aml::equal(last, out)) break;
		if constexpr (Steps != 0) {
			if (stepc == (Steps + 2)) break;
		}

		++stepc;
	}

	return out;
}


}

}