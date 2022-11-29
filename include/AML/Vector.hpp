#pragma once

#include <AML/Iterator.hpp>
#include <AML/Tools.hpp>

#include <cstddef>
#include <type_traits>
#include <algorithm>
#include <tuple>

AML_NAMESPACE

using Vectorsize = std::size_t;

namespace VI
{
	template<Vectorsize I>
	struct Index {};

	inline constexpr Index<0> X;
	inline constexpr Index<1> Y;
	inline constexpr Index<2> Z;
	inline constexpr Index<3> W;
	inline constexpr Index<4> V;
}

namespace detail
{
	template<class T, Vectorsize S>
	struct VectorStorage {
	protected:
		T array[S];
		static constexpr Vectorsize size = S;
	};

	template<class T>
	struct VectorStorage<T, 1> {
		T x;
	protected:
		static constexpr Vectorsize size = 1;
	};

	template<class T>
	struct VectorStorage<T, 2> {
		T x, y;
	protected:
		static constexpr Vectorsize size = 2;
	};

	template<class T>
	struct VectorStorage<T, 3> {
		T x, y, z;
	protected:
		static constexpr Vectorsize size = 3;
	};

	template<class T>
	struct VectorStorage<T, 4> {
		T x, y, z, w;
	protected:
		static constexpr Vectorsize size = 4;
	};

	template<class T>
	struct VectorStorage<T, 5> {
		T x, y, z, w, v;
	protected:
		static constexpr Vectorsize size = 5;
	};

}

template<class T, Vectorsize Size>
class Vector : public detail::VectorStorage<T, Size>
{

	static_assert(std::is_object_v<T>,	  "A vector's T must be an object type");
	static_assert(!std::is_abstract_v<T>, "A vector's T cannot be an abstract class type");
	static_assert(aml::is_complete<T>,	  "A vector's T must be a complete type");

	using Storage = detail::VectorStorage<T, Size>;

public:

	using size_type = Vectorsize;
	using value_type = T;

	using reference = T&;
	using const_reference = const T&;

	static constexpr bool uses_static_array() noexcept {
		return Storage::size > 5;
	}

	using iterator		 = std::conditional_t<uses_static_array(), T* , aml::IndexIterator<Vector<T, Size>>>;
	using const_iterator = std::conditional_t<uses_static_array(), const T*, aml::ConstIndexIterator<Vector<T, Size>>>;

public:

	constexpr
	Vector() noexcept {
		
	}

	static constexpr bool has_index(size_type index) noexcept {
		return Storage::size > index && !uses_static_array();
	}

	constexpr
	Vector(const T (&Array)[Size]) noexcept {
		if constexpr (uses_static_array()) {
			std::copy_n(Array, size(), Storage::array);
		}
		else {
			if constexpr (has_index(0)) Storage::x = Array[0];
			if constexpr (has_index(1)) Storage::y = Array[1];
			if constexpr (has_index(2)) Storage::z = Array[2];
			if constexpr (has_index(3)) Storage::w = Array[3];
			if constexpr (has_index(4)) Storage::v = Array[4];
		}
	}

	template<class First, class... Rest> constexpr
	explicit Vector(First&& f, Rest&&... r) noexcept {
		static_assert(std::is_same_v<T, First> && (std::is_same_v<T, Rest> && ...), "Variadic parameter type is not a storage type");
		static_assert(sizeof...(r) + 1 == Storage::size, "Bad number of variadic parameters");

		if constexpr (uses_static_array()) {
			Storage::array[0] = std::forward<First>(f);
			std::size_t arri = 1;
			([&] {
				Storage::array[arri] = std::forward<Rest>(r);
				++arri;
			}(), ...);
		}
		else {
			auto&& t = std::forward_as_tuple(r...);
			Storage::x = std::forward<First>(f);
			if constexpr (has_index(1)) Storage::y = std::get<0>(t);
			if constexpr (has_index(2)) Storage::z = std::get<1>(t);
			if constexpr (has_index(3)) Storage::w = std::get<2>(t);
			if constexpr (has_index(4)) Storage::v = std::get<3>(t);
		}
	}

	constexpr
	Vector(const Vector&) noexcept = default;

	constexpr
	Vector(Vector&&) noexcept = default;

	constexpr
	Vector& operator=(const Vector&) noexcept = default;

	constexpr
	Vector& operator=(Vector&&) noexcept = default;

	[[nodiscard]] constexpr
	size_type size() const noexcept {
		return Storage::size;
	}

	template<size_type I> constexpr
	reference operator[](VI::Index<I>) noexcept {
		if constexpr (uses_static_array()) {
			return Storage::array[I];
		}
		else {
				 if constexpr (I == 0) return Storage::x;
			else if constexpr (I == 1) return Storage::y;
			else if constexpr (I == 2) return Storage::z;
			else if constexpr (I == 3) return Storage::w;
			else if constexpr (I == 4) return Storage::v;
			AML_UNREACHABLE;
		}
	}

	template<size_type I> constexpr
	const_reference operator[](VI::Index<I>) const noexcept {
		return const_cast<Vector&>(*this)[VI::Index<I>{}];
	}

	constexpr
	reference operator[](size_type index) noexcept {
		if constexpr (uses_static_array()) {
			return Storage::array[index];
		}
		else {
			if constexpr (has_index(0)) if (index == 0) return Storage::x;
			if constexpr (has_index(1)) if (index == 1) return Storage::y;
			if constexpr (has_index(2)) if (index == 2) return Storage::z;
			if constexpr (has_index(3)) if (index == 3) return Storage::w;
			if constexpr (has_index(4)) if (index == 4) return Storage::v;
			AML_UNREACHABLE;
		}
	}

	constexpr
	const_reference operator[](size_type index) const noexcept {
		return const_cast<Vector&>(*this)[index];
	}

	[[nodiscard]] constexpr
	iterator begin() noexcept {
		if constexpr (uses_static_array()) {
			return Storage::array;
		} else {
			return iterator(*this, 0);
		}
	}

	[[nodiscard]] constexpr
	iterator end() noexcept {
		if constexpr (uses_static_array()) {
			return Storage::array + size();
		} else {
			return iterator(*this, size());
		}
	}

	[[nodiscard]] constexpr
	const_iterator begin() const noexcept {
		if constexpr (uses_static_array()) {
			return Storage::array;
		} else {
			return const_iterator(*this, 0);
		}
	}

	[[nodiscard]] constexpr
	const_iterator end() const noexcept {
		if constexpr (uses_static_array()) {
			return Storage::array + size();
		} else {
			return const_iterator(*this, size());
		}
	}

	[[nodiscard]] constexpr
	const_iterator cbegin() const noexcept {
		return begin();
	}

	[[nodiscard]] constexpr
	const_iterator cend() const noexcept {
		return end();
	}

};


#define AML_OP_BODY1(outtype, action) \
	aml::Vector<outtype, Size> out; \
	aml::static_for<Size>([&](Vectorsize i) {	\
		out[i] = action;			\
	});											\
	return out

#define AML_OP_BODY2(action) \
	aml::static_for<Size>([&](Vectorsize i) {	\
		action;									\
	});											\
	return left

template<class Left, class Right, Vectorsize Size> [[nodiscard]] constexpr
auto operator+(const aml::Vector<Left, Size>& left, const aml::Vector<Right, Size>& right) noexcept {
	AML_OP_BODY1(decltype(std::declval<Left>() + std::declval<Right>()), left[i] + right[i]);
}

template<class Left, class Right, Vectorsize Size> constexpr
auto operator+=(aml::Vector<Left, Size>& left, const aml::Vector<Right, Size>& right) noexcept {
	AML_OP_BODY2(left[i] += right[i]);
}

template<class Left, class Right, Vectorsize Size> [[nodiscard]] constexpr
auto operator-(const aml::Vector<Left, Size>& left, const aml::Vector<Right, Size>& right) noexcept {
	AML_OP_BODY1(decltype(std::declval<Left>() - std::declval<Right>()), left[i] - right[i]);
}

template<class Left, class Right, Vectorsize Size> constexpr
auto operator-=(aml::Vector<Left, Size>& left, const aml::Vector<Right, Size>& right) noexcept {
	AML_OP_BODY2(left[i] -= right[i]);
}

template<class Left, class Right, Vectorsize Size> [[nodiscard]] constexpr
auto operator*(const aml::Vector<Left, Size>& left, const Right& right) noexcept {
	AML_OP_BODY1(decltype(std::declval<Left>() * std::declval<Right>()), left[i] * right);
}

template<class Left, class Right, Vectorsize Size> [[nodiscard]] constexpr
auto operator*(const Left& left, const aml::Vector<Right, Size>& right) noexcept {
	return (right * left);
}

template<class Left, class Right, Vectorsize Size> constexpr
auto operator*=(aml::Vector<Left, Size>& left, const Right& right) noexcept {
	AML_OP_BODY2(left[i] *= right);
}

template<class Left, class Right, Vectorsize Size> [[nodiscard]] constexpr
auto operator/(const aml::Vector<Left, Size>& left, const Right& right) noexcept {
	AML_OP_BODY1(decltype(std::declval<Left>() / std::declval<Right>()), left[i] / right);
}

template<class Left, class Right, Vectorsize Size> [[nodiscard]] constexpr
auto operator/(const Left& left, const aml::Vector<Right, Size>& right) noexcept {
	AML_OP_BODY1(decltype(std::declval<Left>() / std::declval<Right>()), left / right[i]);
}

template<class Left, class Right, Vectorsize Size> constexpr
auto operator/=(aml::Vector<Left, Size>& left, const Right& right) noexcept {
	AML_OP_BODY2(left[i] /= right);
}

template<class Left, Vectorsize Size> [[nodiscard]] constexpr
auto operator-(const aml::Vector<Left, Size>& left) noexcept {
	AML_OP_BODY1(Left, -left[i]);
}



template<class Left, class Right, Vectorsize LeftSize, Vectorsize RightSize> [[nodiscard]] constexpr
bool operator==(const aml::Vector<Left, LeftSize>& left, const aml::Vector<Right, RightSize>& right) noexcept {
	if constexpr (LeftSize != RightSize) return false;
	else {
		for (Vectorsize i = 0; i < LeftSize; ++i) {
			if (left[i] != right[i]) return false;
		}
		return true;
	}
}

template<class Left, class Right, Vectorsize LeftSize, Vectorsize RightSize> [[nodiscard]] constexpr
bool operator!=(const aml::Vector<Left, LeftSize>& left, const aml::Vector<Right, RightSize>& right) noexcept {
	return !(left == right);
}


AML_NAMESPACE_END
