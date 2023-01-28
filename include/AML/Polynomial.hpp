#pragma once

#include <AML/Complex.hpp>

#include <utility>
#include <optional>
#include <algorithm>

namespace aml
{

template<class T, std::size_t Deg>
class Polynomial
{
public:

	using value_type = T;
	using size_type = std::size_t;

	using reference = T&;
	using const_reference = const T&;

	using rvalue_reference = T&&;
	using const_rvalue_reference = const T&&;

	using container_type = T[Deg];

	static constexpr size_type degree = Deg;

	
	constexpr
	Polynomial() noexcept {

	}

private:
	template<class... Ts>
	static constexpr bool enable_if_variadic_constructor = (
		!(aml::is_narrowing_conversion<Ts, value_type> || ...)
		&& (sizeof...(Ts) == Deg)
	);
public:

	template<class... Rest, std::enable_if_t<enable_if_variadic_constructor<Rest...>, int> = 0> constexpr
	Polynomial(Rest&&... r) noexcept 
		: container{std::forward<Rest>(r)...}
	{
		AML_DEBUG_VERIFY(!aml::is_zero(*std::rbegin(this->container)), "Eldest coefficient of the polynomial must not be zero");
	}

	constexpr
	Polynomial(const Polynomial&) noexcept = default;

	constexpr
	Polynomial(Polynomial&&) noexcept = default;

	constexpr
	Polynomial& operator=(const Polynomial&) noexcept = default;

	constexpr
	Polynomial& operator=(Polynomial&&) noexcept = default;

	[[nodiscard]] AML_CONSTEVAL static
	size_type size() noexcept {
		return Deg;
	}

	template<size_type I> constexpr reference get() & noexcept { return this->container[I]; }
	template<size_type I> constexpr const_reference get() const & noexcept { return this->container[I]; }
	template<size_type I> constexpr rvalue_reference get() && noexcept { return std::move(*this).container[I]; }
	template<size_type I> constexpr const_rvalue_reference get() const && noexcept { return std::move(*this).container[I]; }

protected:
	container_type container;
};

template<class First, class... Rest>
Polynomial(First&&, Rest&&...) -> Polynomial<
	std::common_type_t<First, Rest...>,
	(1 + sizeof...(Rest))
>;

template<class T, std::size_t RootNumber>
class PolynomialRoot
{
public:

	using value_type = aml::Complex<T>;
	using real_value_type = typename value_type::real_type;

	using size_type = std::size_t;

	using reference = value_type&;
	using const_reference = const value_type&;

	using rvalue_reference = value_type&&;
	using const_rvalue_reference = const value_type&&;

	static constexpr size_type maxrootn = RootNumber;

	using container_type = std::optional<value_type>[maxrootn];

private:
	template<class... Ts>
	static constexpr bool enable_if_variadic_constructor = !(aml::is_narrowing_conversion<Ts, value_type> || ...);
public:

	constexpr
	PolynomialRoot() noexcept {}

	template<class... Rest, std::enable_if_t<enable_if_variadic_constructor<Rest...>, int> = 0> constexpr
	PolynomialRoot(Rest&&... r) noexcept 
		: container{std::forward<Rest>(r)...}
	{
		std::sort(std::begin(this->container), std::end(this->container), [](const auto& a, const auto& b) {
			return (!a.has_value() && b.has_value());
		});
	}

	[[nodiscard]] constexpr
	bool has_roots() const noexcept {
		return this->container[0].has_value();
	}
	[[nodiscard]] constexpr
	bool has_real_roots() const {
		return this->container[0].value().is_real();
	}

	constexpr
	operator const real_value_type&() const {
		return aml::Re(this->container[0].value());
	}

	constexpr
	operator const value_type&() const {
		return this->container[0].value();
	}

	[[nodiscard]] AML_CONSTEVAL static
	size_type size() noexcept {
		return maxrootn;
	}

	void append(const_reference val) noexcept 
	{
		for (auto& i : this->container) {
			if (!i.has_value()) { i = val; return; }
		}
	}

	template<class... Args> 
	void emplace(Args&&... args) noexcept {
		this->append(Complex(std::forward<Args>(args)...));
	}

	template<size_type I> constexpr reference get() & { static_assert(I < maxrootn, "out of range"); return this->container[I].value(); }
	template<size_type I> constexpr const_reference get() const& { static_assert(I < maxrootn, "out of range"); return this->container[I].value(); }
	template<size_type I> constexpr rvalue_reference get() && { static_assert(I < maxrootn, "out of range"); return std::move(std::move(*this).container[I].value()); }
	template<size_type I> constexpr const_rvalue_reference get() const&& { static_assert(I < maxrootn, "out of range"); return std::move(std::move(*this).container[I].value()); }

protected:
	container_type container;
};

template<class... Rest>
PolynomialRoot(Rest&&...) -> PolynomialRoot<
	std::common_type_t<Rest...>, sizeof...(Rest)
>;

namespace detail
{
	template<std::size_t, class>
	struct is_polynomial_with_degree_impl
		: std::false_type {};

	template<std::size_t ExpectDeg, class T>
	struct is_polynomial_with_degree_impl<ExpectDeg, aml::Polynomial<T, ExpectDeg>>
		: std::true_type {};

	template<class>
	struct is_polynomial_impl
		: std::false_type {};

	template<class T, std::size_t Degree>
	struct is_polynomial_impl<aml::Polynomial<T, Degree>>
		: std::true_type {};
}

template<std::size_t ExpectDeg, class T>
inline constexpr bool is_polynomial_with_degree = detail::template is_polynomial_with_degree_impl<ExpectDeg, aml::remove_cvref<T>>::value;

template<class T>
inline constexpr bool is_polynomial = detail::template is_polynomial_impl<T>::value;

template<class T, std::size_t Degree>
using enable_if_polynomial = std::enable_if_t<aml::is_polynomial_with_degree<(Degree + 1), T>, int>;


template<class Pol, enable_if_polynomial<Pol, 0> = 0> [[nodiscard]] constexpr
auto solve([[maybe_unused]] Pol&& polynomial) noexcept 
{
	return 0;
}

template<class Pol, enable_if_polynomial<Pol, 1> = 0> [[nodiscard]] constexpr
auto solve(Pol&& polynomial) noexcept
{
	return PolynomialRoot{ -(get<0>(polynomial) / get<1>(polynomial)) };
}

template<class Pol, enable_if_polynomial<Pol, 2> = 0> [[nodiscard]] constexpr
auto solve(Pol&& polynomial) noexcept
{
	const auto& a = get<2>(polynomial);
	const auto& b = get<1>(polynomial);
	const auto& c = get<0>(polynomial);

	const auto D = sqr(b) - (4 * a * c);

	PolynomialRoot<aml::get_value_type<Pol>, 2> roots;

	if (D <equal> zero) {
		roots.append(-b / (2 * a));
		return roots;
	}

	const auto sqrt_D = aml::csqrt(D);

	roots.append((-b + sqrt_D) / (2 * a));
	roots.append((-b - sqrt_D) / (2 * a));

	return roots;
}





}