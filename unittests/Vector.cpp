
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

TEST(VectorTest, init_filled) {
	Vector<int, 2> a(aml::fill_initializer(10));

	EXPECT_EQ(a[0], 10);
	EXPECT_EQ(a[1], 10);
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

	EXPECT_FLOAT_EQ(aml::dist(a), 5.f);

	const Vector<float, 3> b(1.f, 2.f, 3.f);
	EXPECT_FLOAT_EQ(aml::dist(b), 3.74165738f);

	const Vector<double, 7> c(1., 2., 3., 4., 5., 6., 7.);
	EXPECT_DOUBLE_EQ(aml::dist(c), 11.832159566199232);
}

TEST(VectorTest, dist_between) {
	const Vector<int, 2> a1(15, 21);
	const Vector<int, 2> b1(-5, 0);
	ASSERT_FLOAT_EQ(aml::dist_between(a1, b1), 29.f);

	const Vector<float, 3> a2(100.24f, -100.64f, 50.5f);
	const Vector<float, 3> b2(125.f, 250.f, 300.f);
	ASSERT_FLOAT_EQ(aml::dist_between(a2, b2), 431.05884f);
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

	const Vector<int, 3> i(aml::unit<0>);
	const Vector<int, 3> j(aml::unit<1>);
	const Vector<int, 3> k(aml::unit<2>);

	const auto i_cross_j = aml::cross(i, j);
	EXPECT_EQ(i_cross_j, k);

	const auto j_cross_k = aml::cross(j, k);
	EXPECT_EQ(j_cross_k, i);

	const auto k_cross_i = aml::cross(k, i);
	EXPECT_EQ(k_cross_i, j);
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

TEST(VectorTest, normalize) {
	const Vector<int, 2> a(5, 10);
	const Vector<float, 2> na = aml::normalize(a);

	const Vector<float, 2> resulta(0.44721359549f, 0.894427190999f);
	ASSERT_EQ(na, resulta);
}

TEST(VectorTest, cast_from_dynamic_vector) {
	const DVector<int> a(3, 6, 9);
	const Vector<int, 3> b(a);

	ASSERT_EQ(b.size(), 3);
	EXPECT_EQ(b[0], 3);
	EXPECT_EQ(b[1], 6);
	EXPECT_EQ(b[2], 9);
}

TEST(DynamicVectorTest, init_using_array) {
	const DVector<int> a({ 2 });

	const DVector<int> b({ 3, 4 });
	const DVector<int> c({ 100, -100, 200 });

	const DVector<int> d({ 1, 2, 3, 4, 5, 6 });

	ASSERT_EQ(a.size(), 1);
	ASSERT_EQ(b.size(), 2);
	ASSERT_EQ(c.size(), 3);
	ASSERT_EQ(d.size(), 6);
}

TEST(DynamicVectorTest, cast) {
	const DVector<double> a(1.0, 2.0, 3.0, 4.0, 5.0, 6.0);

	const DVector<int> b(a);

	EXPECT_EQ(a.size(), 6);
	ASSERT_EQ(b.size(), 6);
}

TEST(DynamicVectorTest, init_with_size_initializer) {
	const DVector<int> a(aml::size_initializer(3));

	ASSERT_EQ(a.size(), 3);

	const DVector<float> b(aml::size_initializer(10));

	ASSERT_EQ(b.size(), 10);
}

TEST(DynamicVectorTest, init_with_fill_initializer) 
{
	const DVector<int> a(aml::size_initializer(3), aml::fill_initializer<int>(10));
	ASSERT_EQ(a.size(), 3);

	ASSERT_EQ(a[0], 10);
	ASSERT_EQ(a[1], 10);
	ASSERT_EQ(a[2], 10);

	const DVector<int> b(aml::size_initializer(4), aml::fill_initializer<int>(-3));
	ASSERT_EQ(b.size(), 4);

	ASSERT_EQ(b[0], -3);
	ASSERT_EQ(b[1], -3);
	ASSERT_EQ(b[2], -3);
	ASSERT_EQ(b[3], -3);
}

TEST(DynamicVectorTest, init_with_special_modifiers)
{
	const DVector<int> a(aml::size_initializer(3), aml::one);
	ASSERT_EQ(a.size(), 3);

	EXPECT_EQ(a[0], 1);
	EXPECT_EQ(a[1], 1);
	EXPECT_EQ(a[2], 1);

	const DVector<int> b(aml::size_initializer(2), aml::zero);
	ASSERT_EQ(b.size(), 2);

	EXPECT_EQ(b[0], 0);
	EXPECT_EQ(b[1], 0);

	const DVector<int> c(aml::size_initializer(3), aml::unit<2>);
	ASSERT_EQ(c.size(), 3);

	EXPECT_EQ(c[0], 0);
	EXPECT_EQ(c[1], 0);
	EXPECT_EQ(c[2], 1);
}

TEST(DynamicVectorTest, operators) {
	const DVector<int> a(10, 20);
	const DVector<int> b(-10, -20);

	int sc = 4;

	const auto a_plus_b = a + b;
	const DVector<int> a_plus_b_ans(0, 0);
	EXPECT_EQ(a_plus_b, a_plus_b_ans);

	const auto a_minus_b = a - b;
	const DVector<int> a_minus_b_ans(20, 40);
	EXPECT_EQ(a_minus_b, a_minus_b_ans);

	const auto a_mul_sc = a * sc;
	const DVector<int> a_mul_sc_ans(40, 80);
	EXPECT_EQ(a_mul_sc, a_mul_sc_ans);

	const auto a_div_sc = a / sc;
	const DVector<int> a_div_sc_ans(2, 5);
	EXPECT_EQ(a_div_sc, a_div_sc_ans);

	const auto sc_div_a = sc / a;
	const DVector<int> sc_div_a_ans(0,0);
	EXPECT_EQ(sc_div_a, sc_div_a_ans);

	const auto minus_a = -a;
	const DVector<int> minus_a_ans(-10, -20);
	EXPECT_EQ(minus_a, minus_a_ans);

	auto a_mut = a;

	a_mut += b;
	const DVector<int> a_mut_plus_eq_b_ans(0, 0);
	EXPECT_EQ(a_mut, a_mut_plus_eq_b_ans);

	a_mut -= b;
	const DVector<int> a_mut_minus_eq_b_ans(10, 20);
	EXPECT_EQ(a_mut, a_mut_minus_eq_b_ans);

	a_mut *= sc;
	const DVector<int> a_mut_mul_eq_sc_ans(40, 80);
	EXPECT_EQ(a_mut, a_mut_mul_eq_sc_ans);

	a_mut /= sc;
	const DVector<int> a_mut_div_eq_sc_ans(10, 20);
	EXPECT_EQ(a_mut, a_mut_div_eq_sc_ans);
}

TEST(DynamicVectorTest, functions) {
	const DVector<int> a(1, 3, -2);
	const DVector<int> b(10, 20, 0);

	const auto a_dot_b = aml::dot(a, b);
	EXPECT_EQ(a_dot_b, 70);

	const auto a_cross_b = aml::cross(a, b);
	const DVector<int> a_cross_b_ans(40, -20, -10);
	EXPECT_EQ(a_cross_b, a_cross_b_ans);

	const auto normalize_a = aml::normalize(a);
	const DVector<float> normalize_a_ans(0.267261237f, 0.801783681f, -0.534522474f);
	EXPECT_EQ(normalize_a, normalize_a_ans);

	const auto sum_of_a = aml::sum_of(a);
	const auto sum_of_a_ans = 2;
	EXPECT_EQ(sum_of_a, sum_of_a_ans);

	const auto dist_a = aml::dist(a);
	const auto dist_a_ans = 3.7416573f;
	EXPECT_FLOAT_EQ(dist_a, dist_a_ans);

	const auto dist_between_a_b = aml::dist_between(a, b);
	const auto dist_between_a_b_ans = 19.3390808f;
	EXPECT_FLOAT_EQ(dist_between_a_b, dist_between_a_b_ans);
}

TEST(DynamicVectorTest, cast_from_static_vector) 
{
	const Vector<int, 3> a(10, 20, 30);
	const DVector<int> b(a);

	ASSERT_EQ(b.size(), 3);
	EXPECT_EQ(b[0], 10);
	EXPECT_EQ(b[1], 20);
	EXPECT_EQ(b[2], 30);
}

TEST(DynamicAndStaticVectorTest, operators) 
{
	const DVector<int> a(3, 7);
	const Vector<int, 2> b(1, -4);

	const auto a_plus_b = a + b;
	const DVector<int> a_plus_b_ans(4, 3);
	EXPECT_EQ(a_plus_b, a_plus_b_ans);

	const auto b_plus_a = b + a;
	const DVector<int> b_plus_a_ans(4, 3);
	EXPECT_EQ(b_plus_a, b_plus_a_ans);

	const auto a_minus_b = a - b;
	const DVector<int> a_minus_b_ans(2, 11);
	EXPECT_EQ(a_minus_b, a_minus_b_ans);

	const auto b_minus_a = b - a;
	const DVector<int> b_minus_a_ans(-2, -11);
	EXPECT_EQ(b_minus_a, b_minus_a_ans);

	auto a_mut = a;

	a_mut += b;
	const DVector<int> a_mut_plus_eq_b_ans(4, 3);
	EXPECT_EQ(a_mut, a_mut_plus_eq_b_ans);

	a_mut -= b;
	const DVector<int> a_mut_minus_eq_b_ans(3, 7);
	EXPECT_EQ(a_mut, a_mut_minus_eq_b_ans);

	auto b_mut = b;
	
	b_mut += a;
	const Vector<int, 2> b_mut_plus_eq_a_ans(4, 3);
	EXPECT_EQ(b_mut, b_mut_plus_eq_a_ans);

	b_mut -= a;
	const Vector<int, 2> b_mut_minus_eq_a_ans(1, -4);
	EXPECT_EQ(b_mut, b_mut_minus_eq_a_ans);
}

TEST(DynamicAndStaticVectorTest, functions)
{
	const DVector<int> a(2, -3, 4);
	const Vector<int, 3> b(0, -1, 8);

	const auto a_dot_b = aml::dot(a, b);
	const auto a_dot_b_ans = 35;
	EXPECT_EQ(a_dot_b, a_dot_b_ans);

	const auto a_cross_b = aml::cross(a, b);
	const DVector<int> a_cross_b_ans(-20, -16, -2);
	EXPECT_EQ(a_cross_b, a_cross_b_ans);

	const auto dist_between_a_b = aml::dist_between(a, b);
	const auto dist_between_a_b_ans = 4.89897919f;
	EXPECT_FLOAT_EQ(dist_between_a_b, dist_between_a_b_ans);
}

static_assert(aml::is_vector<Vector<int, 3>>);
static_assert(aml::is_vector<DVector<int>>);
static_assert(!aml::is_vector<int>);

}
