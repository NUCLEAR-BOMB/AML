#pragma once

#include <AML/_AMLCore.hpp>

#include <cstddef>
#include <type_traits>

AML_NAMESPACE

template<class Object>
class IndexIterator
{
public:

	using value_type = typename Object::value_type;

	using difference_type = std::size_t;

	using reference = value_type&;
	using const_reference = const value_type&;

	using pointer = value_type*;

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
	pointer operator->() noexcept {
		return &(m_object[m_shift]);
	}

	[[nodiscard]] constexpr
	reference operator[](difference_type off) noexcept {
		return m_object[m_shift + off];
	}

	[[nodiscard]] constexpr
	const_reference operator[](difference_type off) const noexcept {
		return m_object[m_shift + off];
	}

private:
	difference_type m_shift;
	Object& m_object;
};

template<class Object>
using ConstIndexIterator = aml::IndexIterator<const Object>;


AML_NAMESPACE_END
