#pragma once

#include <AML/_AMLCore.hpp>

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
	using Storage = detail::VectorStorage<T, Size>;
public:

	using size_type = Vectorsize;
	using value_type = T;

	using reference = T&;
	using const_reference = const T&;

	static constexpr bool uses_static_array() noexcept {
		return Storage::size > 5;
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

	template<class... Ts> constexpr
	Vector(Ts&&... r) noexcept {
		static_assert((std::is_same_v<T, Ts> && ...), "Variadic parameter type is not a storage type");
		static_assert(sizeof...(r) == Storage::size, "Bad number of variadic parameters");

		if constexpr (uses_static_array()) {
			std::size_t arri = 0;
			([&] {
				Storage::array[arri] = std::forward<T>(r);
				++arri;
			}(), ...);
		}
		else {
			auto&& t = std::forward_as_tuple(r...);
			if constexpr (has_index(0)) Storage::x = std::get<0>(t);
			if constexpr (has_index(1)) Storage::y = std::get<1>(t);
			if constexpr (has_index(2)) Storage::z = std::get<2>(t);
			if constexpr (has_index(3)) Storage::w = std::get<3>(t);
			if constexpr (has_index(4)) Storage::v = std::get<4>(t);
		}
	}

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

};

AML_NAMESPACE_END
