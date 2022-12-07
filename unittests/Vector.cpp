
#include <AML/Vector.hpp>

#include <gtest/gtest.h>

using namespace aml;

namespace {

TEST(VectorTest, init_using_array) {
	Vector<int, 1> a({ 2 });
	Vector<int, 2> b({ 3, 4 });
	Vector<int, 3> c({ 100, -100, 200 });

	Vector<int, 6> d({ 1, 2, 3, 4, 5, 6 });
}

TEST(VectorTest, init_using_variadic) {
	Vector<int, 2> a(1, 2);
	Vector<int, 3> b(3, 4, 5);

	Vector<int, 6> c(1, 2, 3, 4, 5, 6);
}

TEST(VectorTest, checkSize) {
	Vector<int, 1> a({ 200 });
	EXPECT_EQ(a.size(), 1);

	Vector<int, 2> b({ -5, 1 });
	EXPECT_EQ(b.size(), 2);

	const Vector<int, 4> c({ 0, 1, 2, 3 });
	EXPECT_EQ(c.size(), 4);

	Vector<int, 6> d({ 1, 2, 3, 4, 5, 6 });
	EXPECT_EQ(d.size(), 6);

	const Vector<int, 10> f({ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 });
	EXPECT_EQ(f.size(), 10);
}

TEST(VectorTest, static_call_index_operator) {
	Vector<int, 2> a({ 500, 1000 });
	EXPECT_EQ(a[VI::X], 500);
	EXPECT_EQ(a[VI::Y], 1000);

	const Vector<unsigned int, 3> b({ 300, 800, 200 });
	EXPECT_EQ(b[VI::X], 300);
	EXPECT_EQ(b[VI::Y], 800);
	EXPECT_EQ(b[VI::Z], 200);

	Vector<int, 7> c({ 1, 2, 3, 4, 5, 6, 7 });
	EXPECT_EQ(c[VI::X], 1);
	EXPECT_EQ(c[VI::Y], 2);
	EXPECT_EQ(c[VI::Z], 3);
	EXPECT_EQ(c[VI::index<6>{}], 7);
}

TEST(VectorTest, runtime_call_index_operator) {
	Vector<int, 2> a({ 1, 2 });
	EXPECT_EQ(a[0], 1);
	EXPECT_EQ(a[1], 2);

	Vector<unsigned int, 4> b({ 4, 5, 6, 7 });
	EXPECT_EQ(b[0], 4);
	EXPECT_EQ(b[1], 5);
	EXPECT_EQ(b[2], 6);
	EXPECT_EQ(b[3], 7);

	Vector<int, 8> c({ 1, 2, 3, 4, 5, 6, 7, 8 });
	EXPECT_EQ(c[0], 1);
	EXPECT_EQ(c[1], 2);
	EXPECT_EQ(c[2], 3);
	EXPECT_EQ(c[6], 7);
}

TEST(VectorTest, cast) {
	const Vector<int, 2> a1(10, 20);
	const Vector<float, 2> a2 = static_cast<Vector<float, 2>>(a1);

	const Vector<float, 3> b1(1.f, -2.f, 3.f);
	const Vector<int, 3> b2(b1);
}

TEST(VectorTest, iterate_constant) {
	const Vector<int, 4> a(10, 20, 30, 40);

	std::size_t ai = 0;
	for (const auto& i : a) {
		if (ai == 0) EXPECT_EQ(i, 10);
		else if (ai == 1) EXPECT_EQ(i, 20);
		else if (ai == 2) EXPECT_EQ(i, 30);
		else if (ai == 3) ASSERT_EQ(i, 40);
		++ai;
	}

	ai = 0;
	for (auto i = a.begin(); i != a.end(); ++i) {

		if (ai == 0) EXPECT_EQ(*i, 10);
		else if (ai == 1) EXPECT_EQ(*i, 20);
		else if (ai == 2) EXPECT_EQ(*i, 30);
		else if (ai == 3) ASSERT_EQ(*i, 40);

		++ai;
	}

	const Vector<int, 7> b(13, 23, 33, 43, 53, 63, 73);

	std::size_t bi = 0;
	for (const auto& i : b) {
		if (bi == 0) EXPECT_EQ(i, 13);
		else if (bi == 1) EXPECT_EQ(i, 23);
		else if (bi == 2) EXPECT_EQ(i, 33);
		else if (bi == 3) EXPECT_EQ(i, 43);
		else if (bi == 4) EXPECT_EQ(i, 53);
		else if (bi == 5) EXPECT_EQ(i, 63);
		else if (bi == 6) ASSERT_EQ(i, 73);

		++bi;
	}

	bi = 0;
	for (auto i = b.begin(); i != b.end(); ++i) {
		if (bi == 0) EXPECT_EQ(*i, 13);
		else if (bi == 1) EXPECT_EQ(*i, 23);
		else if (bi == 2) EXPECT_EQ(*i, 33);
		else if (bi == 3) EXPECT_EQ(*i, 43);
		else if (bi == 4) EXPECT_EQ(*i, 53);
		else if (bi == 5) EXPECT_EQ(*i, 63);
		else if (bi == 6) ASSERT_EQ(*i, 73);

		++bi;
	}
}

TEST(VectorTest, iterate) {
	Vector<int, 3> a(10, 20, 30);

	std::size_t ai = 0;
	for (auto& i : a) {
		if (ai == 0) EXPECT_EQ(i, 10);
		else if (ai == 1) EXPECT_EQ(i, 20);
		else if (ai == 2) ASSERT_EQ(i, 30);
		++ai;
	}

	ai = 0;
	for (auto& i : a) {
		i = static_cast<int>(ai * 10);

		if (ai == 0) EXPECT_EQ(i, 0);
		else if (ai == 1) EXPECT_EQ(i, 10);
		else if (ai == 2) ASSERT_EQ(i, 20);
		++ai;
	}

	Vector<int, 7> b(11, 21, 31, 41, 51, 61, 71);

	std::size_t bi = 0;
	for (auto& i : b) {
		if (bi == 0) EXPECT_EQ(i, 11);
		else if (bi == 1) EXPECT_EQ(i, 21);
		else if (bi == 2) EXPECT_EQ(i, 31);
		else if (bi == 3) EXPECT_EQ(i, 41);
		else if (bi == 4) EXPECT_EQ(i, 51);
		else if (bi == 5) EXPECT_EQ(i, 61);
		else if (bi == 6) ASSERT_EQ(i, 71);
		++bi;
	}

	bi = 0;
	for (auto& i : b) {
		i = static_cast<int>(bi * 11);

		if (bi == 0) EXPECT_EQ(i, 0);
		else if (bi == 1) EXPECT_EQ(i, 11);
		else if (bi == 2) EXPECT_EQ(i, 22);
		else if (bi == 3) EXPECT_EQ(i, 33);
		else if (bi == 4) EXPECT_EQ(i, 44);
		else if (bi == 5) EXPECT_EQ(i, 55);
		else if (bi == 6) ASSERT_EQ(i, 66);
		++bi;
	}
}
// +
TEST(VectorTest, add) {
	const Vector<int, 3> a(1, 2, 3);
	const Vector<int, 3> b(3, 2, 1);

	const auto r1 = a + b;
	const auto r2 = b + a;

	const Vector<int, 3> result(4, 4, 4);
	ASSERT_EQ(r1, result);
	ASSERT_EQ(r2, result);
}
// +=
TEST(VectorTest, add_eq) {
	Vector<int, 3> a(2, 4, 6);
	const Vector<int, 3> b(1, 2, 3);

	a += b;

	const Vector<int, 3> result(3, 6, 9);
	ASSERT_EQ(a, result);
}
// -
TEST(VectorTest, sub) {
	const Vector<int, 3> a(3, 4, 5);
	const Vector<int, 3> b(5, 6, 7);

	const auto r1 = a - b;
	const auto r2 = b - a;

	const Vector<int, 3> result(-2, -2, -2);
	ASSERT_EQ(r1, result);
	ASSERT_NE(r1, r2);
}
// -=
TEST(VectorTest, sub_eq) {
	Vector<int, 3> a(-1, -2, -3);
	const Vector<int, 3> b(1, 2, 3);

	a -= b;
	const Vector<int, 3> result(-2, -4, -6);
	ASSERT_EQ(a, result);
}
// *
TEST(VectorTest, mul_by_scalar) {
	const Vector<int, 3> a(0, 1, 0);
	const int b = 3;

	const auto r1 = a * b;
	const auto r2 = b * a;

	const Vector<int, 3> result(0, 3, 0);
	ASSERT_EQ(r1, result);
	ASSERT_EQ(r2, result);
}
// *=
TEST(VectorTest, mul_eq_by_scalar) {
	Vector<int, 3> a(10, 20, 30);
	const int b = 3;

	a *= b;

	const Vector<int, 3> result(30, 60, 90);
	ASSERT_EQ(a, result);
}
// /
TEST(VectorTest, div_by_scalar) {
	const Vector<int, 3> a(5, 10, 15);
	const int b = 5;

	const auto r1 = a / b;
	const auto r2 = b / a;

	const Vector<int, 3> result(1, 2, 3);
	ASSERT_EQ(r1, result);
	ASSERT_NE(r1, r2);
}
// /=
TEST(VectorTest, div_eq_by_scalar) {
	Vector<int, 3> a(10, 20, 30);
	const int b = 10;

	a /= b;

	const Vector<int, 3> result(1, 2, 3);
	ASSERT_EQ(a, result);
}
// -
TEST(VectorTest, neg) {
	const Vector<int, 3> a(1, 3, 5);

	auto b = -a;

	const Vector<int, 3> result(-1, -3, -5);
	ASSERT_EQ(b, result);
	ASSERT_NE(b, a);
}

TEST(VectorTest, dist) {
	const Vector<int, 2> a(3, 4);
	ASSERT_FLOAT_EQ(aml::dist(a), 5.f);

	const Vector<float, 3> b(1.f, 2.f, 3.f);
	ASSERT_FLOAT_EQ(aml::dist(b), 3.74165738f);

	const Vector<double, 7> c(1., 2., 3., 4., 5., 6., 7.);
	ASSERT_DOUBLE_EQ(aml::dist(c), 11.832159566199232);
}

TEST(VectorTest, dist_between) {
	const Vector<int, 2> a1(15, 21);
	const Vector<int, 2> b1(-5, 0);
	ASSERT_FLOAT_EQ(aml::dist(a1, b1), 29.f);

	const Vector<float, 3> a2(100.24f, -100.64f, 50.5f);
	const Vector<float, 3> b2(125.f, 250.f, 300.f);
	ASSERT_FLOAT_EQ(aml::dist(a2, b2), 431.05884f);
}

TEST(VectorTest, dot) {
	const Vector<int, 3> a(5, 6, 7);
	const Vector<int, 3> b(-2, 10, -1);

	ASSERT_EQ(aml::dot(a, b), 43);
}

TEST(VectorTest, cross) {
	const Vector<int, 3> a(1, 2, 3);
	const Vector<int, 3> b(4, 5, 6);

	auto r = aml::cross(a, b);
	const Vector<int, 3> result(-3, 6, -3);
	ASSERT_EQ(r, result);
}

TEST(VectorTest, zerovector) {
	const Vector<int, 3> a(aml::zero);
	const Vector<int, 3> b(0, 0, 0);

	ASSERT_EQ(a, b);
}

TEST(VectorTest, unitvector) {
	const Vector<int, 4> a(aml::unit<0>);

	const Vector<int, 4> result(1, 0, 0, 0);
	ASSERT_EQ(a, result);
}

}
