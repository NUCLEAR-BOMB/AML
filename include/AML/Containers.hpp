#pragma once

#include <AML/Tools.hpp>
#include <functional>

#if AML_CXX20
#include <memory>
#endif

namespace aml
{

template<class T, std::size_t ReserveSize>
class fixed_vector : private std::array<T, ReserveSize>
{
	using Base = std::array<T, ReserveSize>;
public:

	static constexpr auto static_reserved_size = ReserveSize;

	using value_type = typename Base::value_type;
	using size_type = typename Base::size_type;
	using reference = typename Base::reference;
	using const_reference = typename Base::const_reference;
	using iterator = typename Base::iterator;
	using const_iterator = typename Base::const_iterator;
	using reverse_iterator = typename Base::reverse_iterator;
	using const_reverse_iterator = typename Base::const_reverse_iterator;
	using difference_type = typename Base::difference_type;

	using Base::data;

	using Base::begin;
	using Base::cbegin;
	using Base::rend;
	using Base::crend;
	
	using Base::fill;

	constexpr
	fixed_vector() noexcept
		: Base{}
		, m_current_size(0) {}

private:

	template<std::size_t N, std::size_t... Idx> constexpr
	fixed_vector(const value_type (&arr)[N], std::index_sequence<Idx...>) noexcept
		: Base{arr[Idx]...}
		, m_current_size(N) {}

public:

	template<std::size_t N> constexpr
	fixed_vector(const value_type (&arr)[N]) noexcept
		: fixed_vector(arr, std::make_index_sequence<N>{}) {
		//static_assert(N == ReserveSize, "The size of the array must be the same");
	}

	[[nodiscard]] constexpr 
	size_type size() const noexcept { return m_current_size; }

	[[nodiscard]] constexpr
	bool empty() const noexcept {
		return (m_current_size == 0);
	}

	constexpr iterator end() noexcept { return Base::begin() + static_cast<difference_type>(this->size()); }
	constexpr const_iterator end() const noexcept { return Base::begin() + static_cast<difference_type>(this->size()); }
	constexpr const_iterator cend() const noexcept { return this->end(); }

	constexpr reverse_iterator rbegin() noexcept { return Base::rend() - static_cast<difference_type>(this->size()); }
	constexpr const_reverse_iterator rbegin() const noexcept { return this->Base::rend() - static_cast<difference_type>(this->size()); }
	constexpr const_reverse_iterator crbegin() const noexcept { return this->rbegin(); }

	constexpr reference back() noexcept { return (*this)[this->size() - 1]; }
	constexpr const_reference back() const noexcept { return (*this)[this->size() - 1]; }
	constexpr reference front() noexcept { return (*this)[0]; }
	constexpr const_reference front() const noexcept { return (*this)[0]; }

	[[nodiscard]] static AML_CONSTEVAL
	size_type reserved_size() noexcept {
		return ReserveSize;
	}

	template<class Value, std::enable_if_t<std::is_same_v<aml::remove_cvref<Value>, value_type>, int> = 0> constexpr
	void push_back(Value&& value) noexcept
	{
		AML_DEBUG_VERIFY(m_current_size < reserved_size(), "Maximum limit is used");
		Base::operator[](m_current_size) = std::forward<Value>(value);
		++m_current_size;
	}

	template<class... Args> AML_CONSTEXPR20
	void emplace_back(Args&&... args) noexcept 
	{
		AML_DEBUG_VERIFY(m_current_size < reserved_size(), "Maximum limit is used");
		auto* pos = &Base::operator[](m_current_size);

#if AML_CXX20
		std::construct_at(pos, std::forward<Args>(args)...);
#else
		new (pos) value_type(std::forward<Args>(args)...);
#endif
		++m_current_size;
	}

	constexpr
	value_type pop_back() noexcept {
		AML_DEBUG_VERIFY(m_current_size > 0, "Minimum limit is used");
		--m_current_size;
		return Base::operator[](m_current_size);
	}

	constexpr
	void resize(size_type new_size) noexcept {
		AML_DEBUG_VERIFY(new_size < reserved_size(), "Maximum limit is used");
		m_current_size = new_size;
	}
	constexpr
	void clear() noexcept {
		m_current_size = 0;
	}

	constexpr
	reference operator[](size_type index) noexcept {
		AML_DEBUG_VERIFY(index < size(), "Out of range");
		return Base::operator[](index);
	}
	constexpr
	const_reference operator[](size_type index) const noexcept {
		return const_cast<fixed_vector&>(*this)[index];
	}

private:
	size_type m_current_size;
};

template<class Left, std::size_t LeftRSize, class Right, std::size_t RightRSize> [[nodiscard]] constexpr
bool operator==(
	const aml::fixed_vector<Left, LeftRSize>& left,
	const aml::fixed_vector<Right, RightRSize>& right
) noexcept 
{
	if (left.size() != right.size()) return false;

	for (std::size_t i = 0; i < left.size(); ++i) {
		if (!aml::equal(left[i], right[i])) return false;
	}
	return true;
}
#if !AML_CXX20
template<class Left, std::size_t LeftRSize, class Right, std::size_t RightRSize> [[nodiscard]] constexpr
bool operator!=(
	const aml::fixed_vector<Left, LeftRSize>& left,
	const aml::fixed_vector<Right, RightRSize>& right
) noexcept {
	return !(left == right);
}
#endif

template<class T, std::size_t Size>
class fixed_valarray : private std::array<T, Size> {
private:
	using Base = std::array<T, Size>;
public:
	
	static constexpr auto static_reserved_size = Size;

	using value_type = typename Base::value_type;
	using size_type = typename Base::size_type;
	using reference = typename Base::reference;
	using const_reference = typename Base::const_reference;
	using iterator = typename Base::iterator;
	using const_iterator = typename Base::const_iterator;
	using reverse_iterator = typename Base::reverse_iterator;
	using const_reverse_iterator = typename Base::const_reverse_iterator;
	using difference_type = typename Base::difference_type;

	using Base::data;

	using Base::begin;
	using Base::cbegin;
	using Base::rbegin;
	using Base::crbegin;

	using Base::end;
	using Base::cend;
	using Base::rend;
	using Base::crend;

	using Base::back;
	using Base::front;

	using Base::operator[];

	using Base::fill;

	constexpr
	fixed_valarray() noexcept
		: Base{} {}

private:

	template<std::size_t N, std::size_t... Idx> constexpr
	fixed_valarray(const value_type(&arr)[N], std::index_sequence<Idx...>) noexcept
		: Base{arr[Idx]...} {}

public:

	template<std::size_t N> constexpr
	fixed_valarray(const value_type(&arr)[N]) noexcept
		: fixed_valarray(arr, std::make_index_sequence<N>{}) {
		static_assert(N == Size, "The size of the array must be the same");
	}

	constexpr
	fixed_valarray([[maybe_unused]] const aml::fill_initializer<value_type> fill_with) noexcept 
		: Base{} {
		for (auto& i : static_cast<Base&>(*this)) {
			i = fill_with.value;
		}
	}
	constexpr fixed_valarray([[maybe_unused]] const aml::zero_t) noexcept
		: fixed_valarray(aml::fill_initializer<value_type>(aml::zero)) {}
	constexpr fixed_valarray([[maybe_unused]] const aml::one_t) noexcept
		: fixed_valarray(aml::fill_initializer<value_type>(aml::one)) {}


	[[nodiscard]] static AML_CONSTEVAL
	size_type size() noexcept { return Size; }
};

template<class T, std::size_t N>
fixed_valarray(const T(&)[N]) -> fixed_valarray<T, N>;

template<class Left, class Right, std::size_t Size>
struct common_type_body<aml::fixed_valarray<Left, Size>, aml::fixed_valarray<Right, Size>>
{
	using type = aml::fixed_valarray<aml::common_type<Left, Right>, Size>;
};

namespace detail
{
	template<class Operation, class Left, class Right, std::size_t Size> constexpr
	auto do_binary_operation(
		const aml::fixed_valarray<Left, Size>& left, 
		const aml::fixed_valarray<Right, Size>& right
	) noexcept 
	{
		aml::fixed_valarray<
			std::invoke_result_t<Operation, Left, Right>, Size
		> out{};
		for (std::size_t i = 0; i < out.size(); ++i) {
			out[i] = Operation{}(left[i], right[i]);
		}
		return out;	
	}
	template<class Operation, class Left, class Right, std::size_t Size> constexpr
	auto& do_binary_assign_operation(
		aml::fixed_valarray<Left, Size>& left,
		const aml::fixed_valarray<Right, Size>& right
	) noexcept 
	{
		for (std::size_t i = 0; i < left.size(); ++i) {
			Operation{}(left[i], right[i]);
		} 
		return left;
	}

	template<class Operation, bool swap_order, class Left, class Right, std::size_t Size> constexpr
	auto do_binary_by_scalar_operation(const aml::fixed_valarray<Left, Size>& left, const Right& right) noexcept 
	{
		aml::fixed_valarray<
			std::invoke_result_t<Operation, Left, Right>, Size
		> out{};
		for (std::size_t i = 0; i < out.size(); ++i) {
			if constexpr (swap_order) {
				out[i] = Operation{}(right, left[i]);
			} else {
				out[i] = Operation{}(left[i], right);
			}
		}
		return out;
	}
}

#define AML_DEFINE_OP1(op, fun) \
	template<class Left, class Right, std::size_t Size> constexpr	\
	auto operator op (const aml::fixed_valarray<Left, Size>& left, const aml::fixed_valarray<Right, Size>& right) noexcept {	\
		return detail::do_binary_operation< fun >(left, right);	\
	}
#define AML_DEFINE_OP2(op, fun) \
	template<class Left, class Right, std::size_t Size> constexpr	\
	auto& operator op (const aml::fixed_valarray<Left, Size>& left, const aml::fixed_valarray<Right, Size>& right) noexcept {	\
		return detail::do_binary_assign_operation< fun >(left, right);	\
	}
#define AML_DEFINE_OP3(op, fun) \
	template<class Left, class Right, std::size_t Size> constexpr	\
	auto operator op (const aml::fixed_valarray<Left, Size>& left, const Right& right) noexcept {	\
		return detail::do_binary_by_scalar_operation< fun , false>(left, right);	\
	}	\
	template<class Left, class Right, std::size_t Size> constexpr	\
	auto operator op (const Left& left, const aml::fixed_valarray<Right, Size>& right) noexcept {	\
		return detail::do_binary_by_scalar_operation< fun , true>(right, left);	\
	}

AML_DEFINE_OP1(+, aml::plus);
AML_DEFINE_OP1(-, aml::minus);
AML_DEFINE_OP1(*, aml::multiplies);
AML_DEFINE_OP1(/, aml::divides);

AML_DEFINE_OP2(+=, aml::plus_assign);
AML_DEFINE_OP2(-=, aml::minus_assign);
AML_DEFINE_OP2(*=, aml::multiplies_assign);
AML_DEFINE_OP2(/=, aml::divides_assign);

AML_DEFINE_OP3(*, aml::multiplies);
AML_DEFINE_OP3(/, aml::divides);

#undef AML_DEFINE_OP1
#undef AML_DEFINE_OP2
#undef AML_DEFINE_OP3

template<class Left, std::size_t Size> [[nodiscard]] constexpr
auto operator-(const aml::fixed_valarray<Left, Size>& left) noexcept 
{
	aml::remove_cvref<decltype(left)> out{};
	for (std::size_t i = 0; i < left.size(); ++i) {
		out[i] = -left[i];
	}
	return out;
}

template<class Left, std::size_t LeftSize, class Right, std::size_t RightSize> [[nodiscard]] constexpr
bool operator==(const aml::fixed_valarray<Left, LeftSize>& left, const aml::fixed_valarray<Right, RightSize>& right) noexcept {
	if constexpr (LeftSize != RightSize) { return false; }
	else {
		for (std::size_t i = 0; i < left.size(); ++i) {
			if (!aml::equal(left[i], right[i])) return false;
		}
		return true;
	}
}
#if !AML_CXX20
template<class Left, std::size_t LeftSize, class Right, std::size_t RightSize> [[nodiscard]] constexpr
bool operator!=(const aml::fixed_valarray<Left, LeftSize>& left, const aml::fixed_valarray<Right, RightSize>& right) noexcept {
	return !(left == right);
}
#endif

}
