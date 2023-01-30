#pragma once

#include <AML/Tools.hpp>
#include <AML/MathFunctions.hpp>

#include <iostream>
#include <functional>

namespace aml
{

template<class T>
class Complex
{
public:

	friend class Complex;

	using value_type = std::conditional_t<std::is_lvalue_reference_v<T>, 
		std::reference_wrapper<std::remove_reference_t<T>>, 
		T
	>;

	using real_type = value_type;
	using imag_type = value_type;

	static constexpr std::size_t static_size = 2;

	using container_type = value_type[static_size];

	constexpr
	Complex() noexcept {

	}

	template<class Real, class Imag> constexpr
	Complex(Real&& r, Imag&& i) noexcept
		: container{ std::forward<Real>(r), std::forward<Imag>(i) }
	{}

	template<class Real> constexpr
	Complex(Real&& r) noexcept
		: container{ std::forward<Real>(r), static_cast<value_type>(aml::zero) }
	{}

	template<class U> constexpr
	explicit Complex(const Complex<U>& other) noexcept 
		: container{static_cast<value_type>(other.container[RE]), static_cast<value_type>(other.container[IM])}
	{}

	constexpr
	explicit Complex([[maybe_unused]] const aml::zero_t) noexcept
		: container{static_cast<value_type>(aml::zero), static_cast<value_type>(aml::zero)} {}

	constexpr
	explicit Complex([[maybe_unused]] const aml::one_t) noexcept
		: container{ static_cast<value_type>(aml::one), static_cast<value_type>(aml::one) } {}

	template<std::size_t Dir> constexpr
	explicit Complex([[maybe_unused]] const aml::unit_t<Dir>) noexcept
		: container{ (Dir == 0) ? static_cast<value_type>(aml::one) : static_cast<value_type>(aml::zero),
					 (Dir == 1) ? static_cast<value_type>(aml::one) : static_cast<value_type>(aml::zero)} {
		static_assert(Dir < static_size, "Unit direction out of range");
	}

	constexpr
	Complex(const Complex&) noexcept = default;
	constexpr
	Complex(Complex&&) noexcept = default;
	constexpr
	Complex& operator=(const Complex&) noexcept = default;
	constexpr
	Complex& operator=(Complex&&) noexcept = default;

	template<class U> constexpr
	Complex& operator=(const U& other) noexcept {
		this->container[RE] = static_cast<value_type>(other);
		this->container[IM] = static_cast<value_type>(aml::zero);
		return *this;
	}

	[[nodiscard]] constexpr
	bool is_real() const noexcept {
		return aml::is_zero(this->container[IM]);
	}

	template<class U> constexpr
	friend auto& Re(Complex<U>&) noexcept;

	template<class U> constexpr
	friend auto& Im(Complex<U>&) noexcept;

private:
	static constexpr std::size_t RE = 0;
	static constexpr std::size_t IM = 1;

	container_type container;
};

template<class RealT, class ImagT>
Complex(RealT&&, ImagT&&) -> Complex<std::common_type_t<RealT, ImagT>>;

template<class RealT>
Complex(RealT&&) -> Complex<RealT>;

template<class T> constexpr
auto& Re(Complex<T>& c) noexcept { return c.container[Complex<T>::RE]; }
template<class T> constexpr
const auto& Re(const Complex<T>& c) noexcept { return aml::Re(const_cast<Complex<T>&>(c)); }

template<class T> constexpr
decltype(auto) Re(T&& val) noexcept {
	return std::forward<T>(val);
}

template<class T> constexpr
auto& Im(Complex<T>& c) noexcept { return c.container[Complex<T>::IM]; }
template<class T> constexpr
const auto& Im(const Complex<T>& c) noexcept { return aml::Im(const_cast<Complex<T>&>(c)); }

template<class T> constexpr
auto Im([[maybe_unused]] T&&) noexcept {
	return static_cast<T>(aml::zero);
}

template<class T> [[nodiscard]] constexpr
auto to_real(T&& val) noexcept {
	return Complex<T>(std::forward<T>(val), static_cast<T>(aml::zero));
}

template<class T> [[nodiscard]] constexpr
auto to_imag(T&& val) noexcept {
	return Complex<T>(static_cast<T>(aml::zero), std::forward<T>(val));
}

}
// vvv std vvv / ^^^ aml ^^^
namespace std
{
	template<class T>
	struct tuple_size<::aml::Complex<T>>
		: std::integral_constant<std::size_t, ::aml::Complex<T>::static_size> {};

	template<std::size_t I, class T>
	struct tuple_element<I, ::aml::Complex<T>>
	{
		using type = typename ::aml::template Complex<T>::value_type;
	};
}
// ^^^ std ^^^ / vvv aml vvv
namespace aml
{
template<std::size_t I, class T> constexpr
auto& get(Complex<T>& val) noexcept
{
	if constexpr (I == 0) {
		return aml::Re(val);
	} else if constexpr (I == 1) {
		return aml::Im(val);
	}
}
template<std::size_t I, class T> constexpr
const auto& get(const Complex<T>& val) noexcept {
	return aml::get<I>(const_cast<Complex<T>&>(val));
}





template<class T>
inline constexpr bool is_complex = aml::is_specialization<T, aml::Complex>;

// vvvvvv + vvvvvv
template<class Left, class Right, std::enable_if_t<is_complex<Left> || is_complex<Right>, int> = 0> constexpr
auto operator+(const Left& left, const Right& right) noexcept {
	return Complex(Re(left) + Re(right), Im(left) + Im(right));
}
template<class Left, class Right> constexpr
auto& operator+=(Complex<Left>& left, const Right& right) noexcept {
	Re(left) += Re(right);
	Im(left) += Im(right);
	return left;
}
// ^^^^^^ + ^^^^^^

// vvvvvv - vvvvvv
template<class Left, class Right, std::enable_if_t<is_complex<Left> || is_complex<Right>, int> = 0> constexpr
auto operator-(const Left& left, const Right& right) noexcept {
	return Complex(Re(left) - Re(right), Im(left) - Im(right));
}
template<class Left, class Right> constexpr
auto& operator-=(Complex<Left>& left, const Right& right) noexcept {
	Re(left) -= Re(right);
	Im(left) -= Im(right);
	return left;
}
// ^^^^^^ - ^^^^^^

// vvvvvv * vvvvvv
template<class Left, class Right, std::enable_if_t<is_complex<Left> || is_complex<Right>, int> = 0> constexpr
auto operator*(const Left& left, const Right& right) noexcept {
	return Complex(
		(Re(left) * Re(right)) - (Im(left) * Im(right)), 
		(Re(left) * Im(right)) + (Im(left) * Re(right))
	);
}
template<class Left, class Right> constexpr
auto& operator*=(Complex<Left>& left, const Right& right) noexcept {
	left = (left * right);
	return left;
}

// ^^^^^^ * ^^^^^^

// vvvvvv / vvvvvv
template<class Left, class Right, std::enable_if_t<is_complex<Left> || is_complex<Right>, int> = 0> constexpr
auto operator/(const Left& left, const Right& right) noexcept {
	const auto sum_of_sqr = sqr(Re(right)) + sqr(Im(right));
	return Complex(
		(Re(left) * Re(right) + Im(left) * Im(right)) / sum_of_sqr,
		(Im(left) * Re(right) - Re(left) * Im(right)) / sum_of_sqr
	);
}

template<class Left, class Right> constexpr
auto& operator/=(Complex<Left>& left, const Right& right) noexcept {
	left = (left / right);
	return left;
}

// ^^^^^^ / ^^^^^^

template<class Left, class Right, std::enable_if_t<is_complex<Left> || is_complex<Right>, int> = 0> constexpr
bool operator==(const Left& left, const Right& right) noexcept {
	return ( Re(left) <equal> Re(right) ) && ( Im(left) <equal> Im(right) );
}
template<class Left, class Right, std::enable_if_t<is_complex<Left> || is_complex<Right>, int> = 0>
bool operator!=(const Left& left, const Right& right) noexcept {
	return !(left == right);
}

template<class T> [[nodiscard]] constexpr 
auto floor(const Complex<T>& val) noexcept {
	return Complex(aml::floor(Re(val)), aml::floor(Im(val))); }
template<class T> [[nodiscard]] constexpr
auto ceil(const Complex<T>& val) noexcept {
	return Complex(aml::ceil(Re(val)), aml::ceil(Im(val))); }
template<class T> [[nodiscard]] constexpr
auto round(const Complex<T>& val) noexcept {
	return Complex(aml::round(Re(val)), aml::round(Im(val))); }

template<class T> [[nodiscard]] constexpr
auto abs(const Complex<T>& val) noexcept {
	return aml::sqrt( sqr(Re(val)) + sqr(Im(val)) );
}

template<class T> [[nodiscard]] constexpr
auto normalize(const Complex<T>& val) noexcept 
{
	using abstype = decltype(aml::abs(val));

	const abstype inv_mag = static_cast<abstype>(1) / aml::abs(val);
	return (val * inv_mag);
}

template<class T> [[nodiscard]] constexpr
auto csqrt(const T& val) noexcept {
	if (val < 0) {
		return aml::to_imag(aml::sqrt(aml::abs(val)));
	} else {
		return aml::to_real(aml::sqrt(val));
	}
}

template<class T> [[nodiscard]] constexpr
auto sqrt(const Complex<T>& val) noexcept 
{
	const auto r = aml::abs(val);
	const auto val_plus_r = val + r;

	return aml::sqrt(r) * (val_plus_r) / aml::abs(val_plus_r);
}

template<class T> [[nodiscard]] constexpr
auto csqrt(const Complex<T>& val) noexcept {
	return aml::sqrt(val);
}

template<class T>
std::ostream& operator<<(std::ostream& os, const Complex<T>& right) {
	os << '(' << aml::Re(right) << ',' << aml::Im(right) << ")\n";
	return os;
}

}
