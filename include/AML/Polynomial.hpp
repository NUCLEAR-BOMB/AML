#pragma once

#include <AML/Complex.hpp>
#include <AML/Containers.hpp>

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

	static constexpr size_type degree = Deg;

	using container_type = T[degree + 1];

	
	constexpr
	Polynomial() noexcept = default;

private:
	template<class... Ts>
	static constexpr bool enable_if_variadic_constructor = (
		!(aml::is_narrowing_conversion<Ts, value_type> || ...)
		&& (sizeof...(Ts) == (degree + 1))
	);
public:

	template<class... Rest, std::enable_if_t<enable_if_variadic_constructor<Rest...>, int> = 0> constexpr
	Polynomial(Rest&&... r) noexcept 
		: container{std::forward<Rest>(r)...}
	{
		AML_DEBUG_VERIFY(!aml::is_zero(*std::rbegin(this->container)), "Eldest coefficient of the polynomial must not be zero");
	}

	constexpr Polynomial(const Polynomial&) noexcept = default;
	constexpr Polynomial(Polynomial&&) noexcept = default;
	constexpr Polynomial& operator=(const Polynomial&) noexcept = default;
	constexpr Polynomial& operator=(Polynomial&&) noexcept = default;

	[[nodiscard]] AML_CONSTEVAL static
	size_type size() noexcept {
		return std::extent_v<container_type>;
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
	aml::common_type<First, Rest...>, sizeof...(Rest)
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

	using container_type = aml::fixed_vector<value_type, maxrootn>;

private:
	template<class... Ts>
	static constexpr bool enable_if_variadic_constructor = !(aml::is_narrowing_conversion<Ts, value_type> || ...);
public:

	constexpr
	PolynomialRoot() noexcept = default;

	template<class... Rest, std::enable_if_t<enable_if_variadic_constructor<Rest...>, int> = 0> constexpr
	PolynomialRoot(Rest&&... r) noexcept 
		: container({std::forward<Rest>(r)...})
	{
		AML_DEBUG_VERIFY(!aml::equal(this->container.back(), aml::zero), 
			"Can't make a polynomial with the last zero coefficient ");

		for (const auto& root : container) 
		{
			if (root.is_real()) {
				if (!this->has_real_roots()) {
					this->real_root_ptr = &root;
				}
				continue;
			}
			if (!this->has_roots()) {
				this->root_ptr = &root;
			}
			if (this->has_real_roots()) break;
		}

		//std::sort(std::begin(this->container), std::end(this->container), [](const auto& a, const auto& b) {
		//	return (!a.has_value() && b.has_value());
		//});
	}

	constexpr PolynomialRoot(const PolynomialRoot&) noexcept = default;
	constexpr PolynomialRoot(PolynomialRoot&&) noexcept = default;
	constexpr PolynomialRoot& operator=(const PolynomialRoot&) noexcept = default;
	constexpr PolynomialRoot& operator=(PolynomialRoot&&) noexcept = default;

	[[nodiscard]] constexpr
	bool has_roots() const noexcept {
		return this->root_ptr != nullptr;
	}
	[[nodiscard]] constexpr
	bool has_real_roots() const noexcept {
		return this->real_root_ptr != nullptr;
	}

	constexpr
	operator const real_value_type&() const noexcept {
		AML_DEBUG_VERIFY(this->has_real_roots(), "The polynomial has no real roots");

		return aml::Re(*(this->real_root_ptr));
	}

	constexpr
	operator const value_type&() const noexcept {
		AML_DEBUG_VERIFY(this->has_roots(), "The polynomial has no complex roots");

		return *(this->root_ptr);
	}

	[[nodiscard]] constexpr
	size_type rootc() const noexcept {
		return this->container.size();
	}

	constexpr
	void append(const_reference val) noexcept 
	{
		this->container.push_back(val);

		const auto* root = &this->container.back();
		if (!this->has_real_roots() && root->is_real()) {
			real_root_ptr = root; return;
		}
		if (!this->has_roots()) {
			root_ptr = root; return;
		}
	}

	[[nodiscard]] constexpr
	reference last() noexcept {
		return this->container.back();
	}

	template<size_type I> constexpr reference get() & { static_assert(I < maxrootn, "out of range"); return this->container[I]; }
	template<size_type I> constexpr const_reference get() const& { static_assert(I < maxrootn, "out of range"); return this->container[I]; }
	template<size_type I> constexpr rvalue_reference get() && { static_assert(I < maxrootn, "out of range"); return std::move(std::move(*this).container[I]); }
	template<size_type I> constexpr const_rvalue_reference get() const&& { static_assert(I < maxrootn, "out of range"); return std::move(std::move(*this).container[I]); }

protected:
	container_type container;

	const value_type* real_root_ptr = nullptr;
	const value_type* root_ptr = nullptr;
};

template<class... Rest>
PolynomialRoot(Rest&&...) -> PolynomialRoot<
	aml::common_type<Rest...>, sizeof...(Rest)
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
using enable_if_polynomial = std::enable_if_t<aml::is_polynomial_with_degree<(Degree), T>, int>;

#if 0
template<class Pol, enable_if_polynomial<Pol, 0> = 0> [[nodiscard]] constexpr
auto solve([[maybe_unused]] Pol&& polynomial) noexcept 
{
	return 0;
}
#endif

template<class Pol, enable_if_polynomial<Pol, 1> = 0> [[nodiscard]] constexpr
auto solve(Pol&& polynomial) noexcept
{
	return PolynomialRoot{ -(get<0>(polynomial) / get<1>(polynomial)) };
}

template<class Pol, enable_if_polynomial<Pol, 2> = 0> [[nodiscard]] constexpr
auto solve(Pol&& polynomial) noexcept
{
	using poly_val_t = aml::common_type<aml::value_type_of<Pol>, float>;

	auto a = static_cast<poly_val_t>(get<2>(polynomial));
	auto b = static_cast<poly_val_t>(get<1>(polynomial));
	auto c = static_cast<poly_val_t>(get<0>(polynomial));

	const auto D = sqr(b) - (4 * a * c);

	using poly_t = PolynomialRoot<poly_val_t, 2>;

	if (aml::equal(D, aml::zero)) {
		return poly_t{ -b / (2 * a) };
	}

	const auto sqrt_D = aml::csqrt(D);

	return poly_t{(-b + sqrt_D) / (2 * a), (-b - sqrt_D) / (2 * a)};
}



template<class... Args> [[nodiscard]] constexpr
auto solve_polynomial(Args&&... args) noexcept {
	return aml::solve(aml::Polynomial{std::forward<Args>(args)...});
}

}