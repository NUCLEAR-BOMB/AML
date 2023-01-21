#pragma once

#include <AML/Tools.hpp>
#include <AML/MathFunctions.hpp>

namespace aml
{

template<class T>
class Complex
{
public:

	friend class Complex;

	using value_type = T;

	using real_type = value_type;
	using imag_type = value_type;

	static constexpr std::size_t static_size = 2;

	using container_type = T[static_size];

	constexpr
	Complex() noexcept {

	}

	template<class Real, class Imag> constexpr
	Complex(Real&& r, Imag&& i) noexcept
		: container{ std::forward<Real>(r), std::forward<Imag>(i) }
	{}

	template<class Real> constexpr
	Complex(Real&& r) noexcept
		: container{ std::forward<Real>(r), static_cast<value_type>(0) }
	{}

	template<class U> constexpr
	Complex(const Complex<U>& other) noexcept 
		: container{static_cast<T>(other.container[RE]), static_cast<T>(other.container[IM])}
	{}

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
		this->container[RE] = static_cast<T>(other);
		this->container[IM] = static_cast<T>(0);
		return *this;
	}

	template<class T> constexpr
	friend auto& Re(Complex<T>&) noexcept;

	template<class T> constexpr
	friend auto& Im(Complex<T>&) noexcept;

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

template<class T> AML_CONSTEVAL
auto Im([[maybe_unused]] T&&) noexcept {
	return static_cast<T>(0);
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
	return (Re(left) == Re(right)) && (Im(left) == Im(right));
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
	return Complex(aml::ceil(Re(val)), amk::ceil(Im(val))); }
template<class T> [[nodiscard]] constexpr
auto round(const Complex<T>& val) noexcept {
	return Complex(aml::round(Re(val)), aml::round(Im(val))); }

template<class T> [[nodiscard]] constexpr
auto abs(const Complex<T>& val) noexcept {
	return aml::sqrt( sqr(Re(val)) + sqr(Im(val)) );
}

}
