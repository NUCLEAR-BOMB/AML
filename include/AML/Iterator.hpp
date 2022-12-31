#pragma once

#include <AML/_AMLCore.hpp>

#include <cstddef>
#include <type_traits>
#include <iterator>

AML_NAMESPACE

template<class Object, class Difference = std::ptrdiff_t>
class IndexIterator
{
public:

	using iterator_category = std::random_access_iterator_tag;

	using value_type = typename Object::value_type;

	using difference_type = Difference;

	using reference = value_type&;
	using const_reference = const value_type&;

	using pointer = value_type*;
	using const_pointer = const value_type*;

	constexpr
	IndexIterator(Object& object, difference_type startshift) noexcept 
		: m_shift(startshift), m_object(object) {}

	[[nodiscard]] constexpr
	bool operator==(const IndexIterator& right) const noexcept {
		return (m_shift == right.m_shift) && (&m_object == &(right.m_object));
	}

	[[nodiscard]] constexpr
	bool operator!=(const IndexIterator& right) const noexcept {
		return (m_shift != right.m_shift) || (&m_object != &(right.m_object));
	}

	[[nodiscard]] constexpr
	bool operator<(const IndexIterator& right) const noexcept {
		AML_DEBUG_VERIFY(&m_object == &right.m_object, "To compare, you must to have the same object");
		return m_shift < right.m_shift;
	}

	[[nodiscard]] constexpr
	bool operator>(const IndexIterator& right) const noexcept {
		AML_DEBUG_VERIFY(&m_object == &right.m_object, "To compare, you must to have the same object");
		return m_shift > right.m_shift;
	}

	[[nodiscard]] constexpr
	bool operator<=(const IndexIterator& right) const noexcept {
		AML_DEBUG_VERIFY(&m_object == &right.m_object, "To compare, you must to have the same object");
		return m_shift <= right.m_shift;
	}

	[[nodiscard]] constexpr
	bool operator>=(const IndexIterator& right) const noexcept {
		AML_DEBUG_VERIFY(&m_object == &right.m_object, "To compare, you must to have the same object");
		return m_shift >= right.m_shift;
	}

	constexpr
	IndexIterator& operator+=(difference_type off) noexcept {
		m_shift += off; return *this;
	}

	constexpr
	IndexIterator& operator-=(difference_type off) noexcept {
		m_shift -= off; return *this;
	}

	constexpr
	IndexIterator& operator++() noexcept {
		++m_shift; return *this;
	}

	[[nodiscard]] constexpr
	IndexIterator operator++(int) noexcept {
		IndexIterator tmp(*this);
		++m_shift;
		return tmp;
	}

	constexpr
	IndexIterator& operator--() noexcept {
		--m_shift; return *this;
	}

	[[nodiscard]] constexpr
	IndexIterator operator--(int) noexcept {
		IndexIterator tmp(*this);
		--m_shift;
		return tmp;
	}

	[[nodiscard]] constexpr
	IndexIterator operator+(difference_type off) const noexcept {
		IndexIterator tmp(*this);
		tmp += off;
		return tmp;
	}

	[[nodiscard]] constexpr
	IndexIterator operator-(difference_type off) const noexcept {
		IndexIterator tmp(*this);
		tmp -= off;
		return tmp;
	}

	[[nodiscard]] constexpr
	difference_type operator-(const IndexIterator& right) const noexcept {
		return (m_shift - right.m_shift);
	}

	[[nodiscard]] constexpr
	std::conditional_t<std::is_const_v<Object>, const_reference, reference> operator*() noexcept {
		return m_object[m_shift];
	}

	[[nodiscard]] constexpr
	std::conditional_t<std::is_const_v<Object>, const_pointer, pointer> operator->() noexcept {
		return std::addressof(m_object[m_shift]);
	}

	[[nodiscard]] constexpr
	std::conditional_t<std::is_const_v<Object>, const_reference, reference> operator[](difference_type off) noexcept {
		return m_object[m_shift + off];
	}

private:
	difference_type m_shift;
	Object& m_object;
};

template<class Object, class Difference = std::ptrdiff_t>
using ConstIndexIterator = aml::IndexIterator<const Object, Difference>;


AML_NAMESPACE_END
