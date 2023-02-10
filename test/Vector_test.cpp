
#include "Testing.hpp"

#include <AML/Vector.hpp>

namespace {

DEFINE_TEST(vector_construct)
{
	DEFINE_VAR aml::Vector<int, 2> vec1(100, 200);
	TEST_EQUALS(vec1.x, 100);
	TEST_EQUALS(vec1.y, 200);

	TEST_EQUALS(vec1.size(), 2);

	DEFINE_VAR aml::Vector vec2(123.f, 456.f);
	TEST_EQUALS(vec2[aml::VI::X], 123.f);
	TEST_EQUALS(vec2[aml::VI::Y], 456.f);
	TEST_EQUALS(vec2.size(), 2);

	DEFINE_VAR aml::Vector vec3(1, 2, 3);
	TEST_EQUALS(vec3[0], 1);
	TEST_EQUALS(vec3[1], 2);
	TEST_EQUALS(vec3[2], 3);
	TEST_EQUALS(vec3.size(), 3);

	DEFINE_VAR aml::Vector vec4({1., 1e100, 2e-200});
	TEST_EQUALS(vec4.x, 1.);
	TEST_EQUALS(vec4.y, 1e100);
	TEST_EQUALS(vec4.z, 2e-200);
	TEST_EQUALS(vec4.size(), 3);

	DEFINE_VAR aml::Vector vec5(1, 2, 3, 4, 5, 6, 7, 8, 9);
	TEST_EQUALS(vec5[aml::index_v<7>], 8);
	TEST_EQUALS(vec5[aml::VI::X], 1);

	TEST_EQUALS(vec5.size(), 9);
}

DEFINE_TEST(vector_copy)
{
	DEFINE_VAR aml::Vector vec1(1, -2, 3, -4);
	TEST_EQUALS(vec1.x, 1);
	TEST_EQUALS(vec1.y, -2);
	TEST_EQUALS(vec1.z, 3);
	TEST_EQUALS(vec1.w, -4);
	TEST_EQUALS(vec1.size(), 4);

	DEFINE_VAR aml::Vector vec2 = vec1;
	TEST_EQUALS(vec1, vec2);

	TEST_EQUALS(vec2.x, 1);
	TEST_EQUALS(vec2.y, -2);
	TEST_EQUALS(vec2.z, 3);
	TEST_EQUALS(vec2.w, -4);
	TEST_EQUALS(vec2.size(), 4);
}

DEFINE_TEST(vector_fill)
{
	DEFINE_VAR aml::Vector<int, 3> vec1(aml::fill_initializer(123));
	TEST_EQUALS(vec1[0], 123);
	TEST_EQUALS(vec1[1], 123);
	TEST_EQUALS(vec1[2], 123);

	DEFINE_VAR aml::Vector<int, 3> vec3(1, 2, 3);

	DEFINE_VAR aml::Vector<float, 20> vec2(aml::fill_initializer(1.2f));
}

DEFINE_TEST(vector_cast)
{
	DEFINE_VAR aml::Vector vec1(1.f, 2.f, 3.f);

	DEFINE_VAR auto vec2 = static_cast<aml::Vector<int, 3>>(vec1);

	TEST_EQUALS(vec2, aml::Vector(1, 2, 3));
}

DEFINE_TEST(vector_iterate)
{
	FORCE_COMPILE_TIME({
	const aml::Vector vec(10, 20, 30, 40, 50, 60);
		
	std::size_t index = 0;
	for (const auto& i : vec)
	{
		if (index == 0 && i != 10) throw 0;
		if (index == 1 && i != 20) throw 0;
		if (index == 2 && i != 30) throw 0;
		if (index == 3 && i != 40) throw 0;
		if (index == 4 && i != 50) throw 0;
		if (index == 5 && i != 60) throw 0;

		++index;
	}
	});

	FORCE_COMPILE_TIME({
	const aml::Vector vec(1, 2, 3);

	std::size_t index = 0;
	for (const auto& i : vec)
	{
		if (index == 0 && i != 1) throw 0;
		if (index == 1 && i != 2) throw 0;
		if (index == 2 && i != 3) throw 0;

		++index;
	}
	});

	FORCE_COMPILE_TIME({
	aml::Vector vec(1, 2, 3, 4, 5, 6);
	
	for ([[maybe_unused]] auto& i : vec) {}
	});

	FORCE_COMPILE_TIME({
	aml::Vector vec(1, 2);

	for ([[maybe_unused]] auto& i : vec) {}
	});
}

DEFINE_TEST(vector_operators)
{
	DEFINE_VAR aml::Vector vecA(1, 2);
	DEFINE_VAR aml::Vector vecB(3.f, -2.f);

	TEST_EQUALS((vecA + vecB),	aml::Vector(4.f, 0.f));
	TEST_EQUALS((vecB + vecA),	(vecA + vecB));

	TEST_EQUALS((vecA - vecB),	aml::Vector(-2.f, 4.f));
	TEST_EQUALS((vecB - vecA),	-(vecA - vecB));

	TEST_EQUALS((vecA * 1.5f),	aml::Vector(1.5f, 3.f));
	TEST_EQUALS((1.5f * vecA),	(vecA * 1.5f));

	TEST_EQUALS((vecB / 2),		aml::Vector(1.5f, -1.f));
	TEST_EQUALS((2 / vecB),		(1 / (vecB / 2)));

	TEST_EQUALS([&]() {
		aml::Vector vec = vecB;
		vec += vecA;
		vec -= aml::Vector(3.f, 100.f);
		return vec;
	}(), aml::Vector(1.f, -100.f));

	TEST_EQUALS([&]() {
		aml::Vector vec(1.f, 0.f);
		vec *= 1e20f;
		vec /= 1e10f;
		return vec;
	}(), aml::Vector(1e10f, 0.f));

	TEST_EQUALS(-vecA, aml::Vector(-1, -2));
}

DEFINE_TEST(vector_functions)
{
	TEST_EQUALS(aml::dist(aml::Vector(3, 4)), 5.f);

	TEST_EQUALS(aml::dist_between(aml::Vector(1, 1), aml::Vector(1, 2)), 1.f);

	TEST_EQUALS(aml::normalize(aml::Vector(1, 0)), aml::Vector(1.f, 0.f));
	TEST_EQUALS(aml::normalize(aml::Vector(0, 1)), aml::Vector(0.f, 1.f));

	TEST_EQUALS(aml::dot(aml::Vector(5, 6, 7), aml::Vector(-2, 10, -1)), 43);

	using aml::dot;
	TEST_EQUALS(aml::Vector(5, 6, 7) <dot> aml::Vector(-2, 10, -1), 43);

	using aml::cross;
	TEST_EQUALS(aml::Vector(1, 2, 3) <cross> aml::Vector(4, 5, 6), aml::Vector(-3, 6, -3));

	DEFINE_VAR aml::Vector<int, 3> i(aml::unit<0>);
	DEFINE_VAR aml::Vector<int, 3> j(aml::unit<1>);
	DEFINE_VAR aml::Vector<int, 3> k(aml::unit<2>);

	TEST_EQUALS(i <cross> j, k);
	TEST_EQUALS(j <cross> k, i);
	TEST_EQUALS(k <cross> i, j);
}

DEFINE_TEST(vector_special_init)
{
	TEST_EQUALS((aml::Vector<int, 3>(aml::zero)),	 aml::Vector(0, 0, 0));

	TEST_EQUALS((aml::Vector<int, 4>(aml::unit<0>)), aml::Vector(1, 0, 0, 0));
	TEST_EQUALS((aml::Vector<int, 4>(aml::unit<2>)), aml::Vector(0, 0, 1, 0));

	TEST_EQUALS((aml::Vector<int, 2>(aml::one)),	 aml::Vector(1, 1));
}

DEFINE_TEST(vector_resize)
{
	DEFINE_VAR aml::Vector vec1(1, 2, 3, 4);

	TEST_EQUALS(vec1.resize<2>(), aml::Vector(1, 2));
	TEST_EQUALS(vec1.resize<6>(), aml::Vector(1, 2, 3, 4, 0, 0));
	TEST_EQUALS((vec1.resize<3, float>()), aml::Vector(1.f, 2.f, 3.f));
}

DEFINE_TEST(vector_to_array)
{
	DEFINE_VAR auto arr = aml::Vector(1, 2, 3).to_array();

	TEST_EQUALS(arr[0], 1);
	TEST_EQUALS(arr[1], 2);
	TEST_EQUALS(arr[2], 3);
}

DEFINE_TEST(vector_structured_binding)
{
	FORCE_COMPILE_TIME({
		aml::Vector vec(1e3, 1e6, 1e9);

		auto& [x, y, z] = vec;

		x = 0;
		y = 0;
		z = 1;

		if (vec != aml::Vector(0., 0., 1.)) throw 0;
	});
}

DEFINE_TEST(vector_reference_type)
{
	FORCE_COMPILE_TIME({
		int x = 10, y = 20;

		aml::Vector<int&, 2> vec(x, y);

		vec[0] = 20;
		if (x != 20) throw 0;

		vec[1] = -10;
		if (y != -10) throw 0;
	});
}

static_assert(std::is_same_v<
	aml::common_type<aml::Vector<int, 4>, aml::Vector<float, 3>, aml::Vector<long long, 1>>, 
	aml::Vector<float, 4>
>);

static_assert(std::is_same_v<
	aml::common_type<aml::DVector<int>, aml::DVector<float>>,
	aml::DVector<float>
>);

static_assert(std::is_same_v<
	aml::common_type<aml::Vector<unsigned, 2>, aml::DVector<long>>,
	aml::DVector<unsigned long>
>);

static_assert(std::is_same_v<
	aml::common_type<aml::DVector<float>, aml::Vector<double, 100>>,
	aml::DVector<double>
>);

}
