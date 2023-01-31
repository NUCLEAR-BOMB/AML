#pragma once

#include <AML/Functions.hpp>

namespace aml
{
namespace algorithms
{

template<class T, class E> [[nodiscard]] constexpr
T binary_pow(T val, E exp) noexcept
{
	static_assert(!std::is_floating_point_v<E>, "The exponent type must be a non floating point type");
	
	AML_DEBUG_VERIFY(exp >= 0, "The exponent must not be less than 0");

	T out = static_cast<T>(1);

	while (exp != 0)
	{
		if (aml::odd(exp)) {
			out *= val;
		}
		val *= val;
		exp >>= 1; // exp /= 2
	}
	return out;
}

template<class T> [[nodiscard]]
auto fast_precise_pow(const T& val, const T& exp) noexcept
{
	static_assert(std::numeric_limits<T>::is_iec559, "Requires IEEE-comliant for T");

	using integer_t = aml::unsigned_from_bytes<sizeof(T)>;
	static_assert(sizeof(integer_t) == sizeof(T));

	constexpr auto mantisa = integer_t(1) << (std::numeric_limits<T>::digits - 1);
	constexpr auto exponent = [&]() 
	{
		     if constexpr (std::is_same_v<T, float>)  return 127;
		else if constexpr (std::is_same_v<T, double>) return 1024;
		else {
			return std::numeric_limits<T>::max_exponent;
		}
	}();
	constexpr auto approx_constant = [&]() 
	{
		     if constexpr (std::is_same_v<T, float>)  return 0.0450465f;
		else if constexpr (std::is_same_v<T, double>) return 1.0730088;
		else {
			return 0;
		}
	}();
	constexpr auto magic_constant = static_cast<integer_t>(mantisa * (exponent - approx_constant));

	integer_t i;
	std::memcpy(&i, &val, sizeof(T));

	i = static_cast<integer_t>(magic_constant + exp * (i - magic_constant));

	T out;
	std::memcpy(&out, &i, sizeof(T));

	return out;
}

[[nodiscard]] inline
double fast_pow(double val, double exp) noexcept
{
	// https://martin.ankerl.com/2012/01/25/optimized-approximative-pow-in-c-and-cpp/

	using integer_t = std::int32_t;
	static_assert(sizeof(integer_t) == (sizeof(val) / 2));

	integer_t x[2];
	std::memcpy(&x, &val, sizeof(x));
	x[1] = static_cast<integer_t>(exp * (x[1] - 1072632447) + 1072632447);
	x[0] = 0;
	std::memcpy(&val, &x, sizeof(x));
	return val;
}

[[nodiscard]] inline
double fast_precise_pow2(double val, double exp) noexcept
{
	AML_DEBUG_VERIFY(exp >= 1.0, "The exponent must be larger or equal to 1");

	using integer_t = std::int32_t;
	static_assert(sizeof(integer_t) == (sizeof(val) / 2));

	const auto e = static_cast<integer_t>(exp);

	const auto x = aml::algorithms::fast_pow(val, (exp - e));
	const auto r = aml::algorithms::binary_pow(val, e);

	return r * x;

}



}

}