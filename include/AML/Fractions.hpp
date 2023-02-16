#pragma once

#include <AML/Tools.hpp>
#include <AML/Functions.hpp>
#include <numeric>
#include <utility>

namespace aml
{


template<class NumeratorType, class DenominatorType = NumeratorType>
class Fraction
{
public:

	using numerator_type = NumeratorType;
	using denominator_type = DenominatorType;

	using value_type = aml::common_type<numerator_type, denominator_type>;

	constexpr
	explicit Fraction(const numerator_type& numer, const denominator_type& denom) noexcept
		: numerator_value(numer), denominator_value(denom) {}
	constexpr
	explicit Fraction(const numerator_type& numer) noexcept
		: numerator_value(numer), denominator_value(static_cast<denominator_type>(aml::zero)) {}

	constexpr numerator_type& numerator() noexcept {
		return this->numerator_value;
	}
	constexpr const numerator_type& numerator() const noexcept {
		return const_cast<Fraction&>(*this).numerator();
	}

	constexpr denominator_type& denominator() noexcept {
		return this->denominator_value;
	}
	constexpr const denominator_type& denominator() const noexcept {
		return const_cast<Fraction&>(*this).denominator();
	}

private:

	template<class ConvType> constexpr
	auto cast_impl() const noexcept {
		return static_cast<ConvType>(
			static_cast<ConvType>(numerator()) / static_cast<ConvType>(denominator())
		);
	}

public:

	template<class Floating, std::enable_if_t<!std::is_integral_v<Floating>, int> = 0> constexpr
	operator Floating() const noexcept {
		return this->cast_impl<Floating>();
	}

	template<class Integer, std::enable_if_t<std::is_integral_v<Integer>, int> = 0> constexpr
	explicit operator Integer() const noexcept {
		return this->cast_impl<Integer>();
	}


private:
	numerator_type numerator_value;
	denominator_type denominator_value;
};

template<class Numer, class Denom>
Fraction(Numer, Denom) -> Fraction<Numer, Denom>;

template<class Numer>
Fraction(Numer) -> Fraction<Numer, Numer>;

template<class Numer, class Denom> constexpr
auto simplify(const Fraction<Numer, Denom>& fract) noexcept
{
	const auto gcd_ = std::gcd(fract.numerator(), fract.denominator());
	return Fraction(fract.numerator() / gcd_, fract.denominator() / gcd_);
}

template<class NumerLeft, class DenomLeft, class NumerRight, class DenomRight> constexpr
auto common_denominator(const Fraction<NumerLeft, DenomLeft>& left, const Fraction<NumerRight, DenomRight>& right) noexcept
{
	return std::pair(
		Fraction(left.numerator() * right.denominator(), left.denominator() * right.denominator()),
		Fraction(right.numerator() * left.denominator(), right.denominator() * left.denominator())
	);
}


namespace detail
{
	template<class Operation, class NumerLeft, class DenomLeft, class NumerRight, class DenomRight> constexpr
	bool do_fraction_compare(const Fraction<NumerLeft, DenomLeft>& left, const Fraction<NumerRight, DenomRight>& right) noexcept 
	{
		return Operation{}(left.numerator() * right.denominator(), left.denominator() * right.numerator());
	}
}

#if !AML_CXX20
template<class NumerLeft, class DenomLeft, class NumerRight, class DenomRight> constexpr
bool operator!=(const Fraction<NumerLeft, DenomLeft>& left, const Fraction<NumerRight, DenomRight>& right) noexcept
{
	return !(left == right);
}
#endif

#define AML_DEFINE_COMP_OP1(op, functor) \
	template<class NumerLeft, class DenomLeft, class NumerRight, class DenomRight> constexpr								\
	bool operator op (const Fraction<NumerLeft, DenomLeft>& left, const Fraction<NumerRight, DenomRight>& right) noexcept {	\
		return detail::do_fraction_compare< functor >(left, right);															\
	}

AML_DEFINE_COMP_OP1(==, decltype(aml::equal))
AML_DEFINE_COMP_OP1(>,  aml::greater)
AML_DEFINE_COMP_OP1(>=, aml::greater_equal)
AML_DEFINE_COMP_OP1(< , aml::less)
AML_DEFINE_COMP_OP1(<=, aml::less_equal)

#undef AML_DEFINE_COMP_OP1

#define AML_DEFINE_COMP_OP2(op) \
	template<class Numer, class Denom, class Other> constexpr							\
	auto operator op (const Fraction<Numer, Denom>& left, const Other& right) noexcept {\
		return static_cast<Other>(left) op right;										\
	}																					\
	template<class Numer, class Denom, class Other> constexpr							\
	auto operator op (const Other& left, const Fraction<Numer, Denom>& right) noexcept {\
		return left op static_cast<Other>(right);										\
	}
	
AML_DEFINE_COMP_OP2(>)
AML_DEFINE_COMP_OP2(>=)
AML_DEFINE_COMP_OP2(<)
AML_DEFINE_COMP_OP2(<=)
AML_DEFINE_COMP_OP2(==)
AML_DEFINE_COMP_OP2(!=)

#undef AML_DEFINE_COMP_OP2

template<class NumerLeft, class DenomLeft, class NumerRight, class DenomRight> constexpr
auto operator*(const Fraction<NumerLeft, DenomLeft>& left, const Fraction<NumerRight, DenomRight>& right) noexcept {
	return Fraction(left.numerator() * right.numerator(), left.denominator() * right.denominator());
}
template<class Numer, class Denom, class Right> constexpr
auto operator*(const Fraction<Numer, Denom>& left, const Right& right) noexcept {
	return Fraction(left.numerator() * right, left.denominator());
}
template<class Numer, class Denom, class Left> constexpr
auto operator*(const Left& left, const Fraction<Numer, Denom>& right) noexcept { return (right * left); }

template<class NumerLeft, class DenomLeft, class NumerRight, class DenomRight> constexpr
auto operator/(const Fraction<NumerLeft, DenomLeft>& left, const Fraction<NumerRight, DenomRight>& right) noexcept {
	return Fraction(left.numerator() * right.denominator(), left.denominator() * right.numerator());
}
template<class Numer, class Denom, class Right> constexpr
auto operator/(const Fraction<Numer, Denom>& left, const Right& right) noexcept {
	return Fraction(left.numerator(), left.denominator() * right);
}
template<class Numer, class Denom, class Left> constexpr
auto operator/(const Left& left, const Fraction<Numer, Denom>& right) noexcept { 
	return Fraction(right.denominator() * left, right.numerator());
}

namespace detail
{
	template<class Operation, bool swap_order, class Numer, class Denom, class Right> constexpr
	auto do_fraction_operator(const Fraction<Numer, Denom>& left, const Right& right) noexcept
	{
		return Fraction(
			aml::swap_binary_args_if<Operation, swap_order>{}(
				left.numerator(), right * left.denominator()
			), 
			left.denominator()
		);
	}
	template<class Operation, class NumerLeft, class DenomLeft, class NumerRight, class DenomRight> constexpr
	auto do_fraction_operator(const Fraction<NumerLeft, DenomLeft>& left, const Fraction<NumerRight, DenomRight>& right) noexcept
	{
		return Fraction(
			Operation{}(
				left.numerator() * right.denominator(), 
				right.numerator() * left.denominator()
			),
			left.denominator() * right.denominator()
		);
	}

}

template<class NumerLeft, class DenomLeft, class NumerRight, class DenomRight> constexpr
auto operator+(const Fraction<NumerLeft, DenomLeft>& left, const Fraction<NumerRight, DenomRight>& right) noexcept {
	return detail::do_fraction_operator<aml::plus>(left, right);
}
template<class Numer, class Denom, class Right> constexpr
auto operator+(const Fraction<Numer, Denom>& left, const Right& right) noexcept {
	return detail::do_fraction_operator<aml::plus, false>(left, right);
}
template<class Numer, class Denom, class Left> constexpr
auto operator+(const Left& left, const Fraction<Numer, Denom>& right) noexcept { 
	return detail::do_fraction_operator<aml::plus, true>(right, left);
}

template<class NumerLeft, class DenomLeft, class NumerRight, class DenomRight> constexpr
auto operator-(const Fraction<NumerLeft, DenomLeft>& left, const Fraction<NumerRight, DenomRight>& right) noexcept {
	return detail::do_fraction_operator<aml::minus>(left, right);
}
template<class Numer, class Denom, class Right> constexpr
auto operator-(const Fraction<Numer, Denom>& left, const Right& right) noexcept {
	return detail::do_fraction_operator<aml::minus, false>(left, right);
}
template<class Numer, class Denom, class Left> constexpr
auto operator-(const Left& left, const Fraction<Numer, Denom>& right) noexcept { 
	return detail::do_fraction_operator<aml::minus, true>(right, left);
}

template<class Numer, class Denom> constexpr
auto operator-(const Fraction<Numer, Denom>& left) noexcept {
	return Fraction(-left.numerator(), left.denominator());
}




}