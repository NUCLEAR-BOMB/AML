
#include <AML/Vector.hpp>

#include <gtest/gtest.h>

namespace {

TEST(dynamic_vector_test, cast_from_dynamic_vector) {
	const aml::DVector<int> vec1(3, 6, 9);
	const aml::Vector<int, 3> vec2 = static_cast<aml::Vector<int, 3>>(vec1);

	ASSERT_EQ(vec2.size(), 3);
	EXPECT_EQ(vec2[0], 3);
	EXPECT_EQ(vec2[1], 6);
	EXPECT_EQ(vec2[2], 9);
}

#if AML_CXX20 && defined(AML_PACK_VECTOR)
TEST(dynamic_vector_test, to_span) {

	aml::Vector a(1, 2, 3, 4, 5, 6);

	auto a1 = a.to_span();

	const aml::Vector b(1, 2, 3, 4, 5);

	auto b1 = b.to_span();
}
#endif

TEST(dynamic_vector_test, init_using_array) {
	const aml::DVector<int> a({ 2 });

	const aml::DVector<int> b({ 3, 4 });
	const aml::DVector<int> c({ 100, -100, 200 });

	const aml::DVector<int> d({ 1, 2, 3, 4, 5, 6 });

	ASSERT_EQ(a.size(), 1);
	ASSERT_EQ(b.size(), 2);
	ASSERT_EQ(c.size(), 3);
	ASSERT_EQ(d.size(), 6);
}

TEST(dynamic_vector_test, cast) {
	const aml::DVector<double> a(1.0, 2.0, 3.0, 4.0, 5.0, 6.0);

	const aml::DVector<int> b(a);

	EXPECT_EQ(a.size(), 6);
	ASSERT_EQ(b.size(), 6);
}

TEST(dynamic_vector_test, init_with_size_initializer) {
	const aml::DVector<int> a(aml::size_initializer(3));

	ASSERT_EQ(a.size(), 3);

	const aml::DVector<float> b(aml::size_initializer(10));

	ASSERT_EQ(b.size(), 10);
}

TEST(dynamic_vector_test, init_with_fill_initializer) 
{
	const aml::DVector<int> a(aml::size_initializer(3), aml::fill_initializer<int>(10));
	ASSERT_EQ(a.size(), 3);

	ASSERT_EQ(a[0], 10);
	ASSERT_EQ(a[1], 10);
	ASSERT_EQ(a[2], 10);

	//const aml::DVector<int> b(aml::size_initializer(4), aml::fill_initializer<int>(-3));
	const aml::DVector<int> b(4, aml::fill_initializer<int>(-3));

	ASSERT_EQ(b.size(), 4);

	ASSERT_EQ(b[0], -3);
	ASSERT_EQ(b[1], -3);
	ASSERT_EQ(b[2], -3);
	ASSERT_EQ(b[3], -3);
}

TEST(dynamic_vector_test, init_with_special_modifiers)
{
	const aml::DVector<int> a(aml::size_initializer(3), aml::one);
	ASSERT_EQ(a.size(), 3);

	EXPECT_EQ(a[0], 1);
	EXPECT_EQ(a[1], 1);
	EXPECT_EQ(a[2], 1);

	const aml::DVector<int> b(aml::size_initializer(2), aml::zero);
	ASSERT_EQ(b.size(), 2);

	EXPECT_EQ(b[0], 0);
	EXPECT_EQ(b[1], 0);

	const aml::DVector<int> c(aml::size_initializer(3), aml::unit<2>);
	ASSERT_EQ(c.size(), 3);

	EXPECT_EQ(c[0], 0);
	EXPECT_EQ(c[1], 0);
	EXPECT_EQ(c[2], 1);
}

TEST(dynamic_vector_test, init_resized)
{
	const auto a = aml::DVector<int>(1, 2, 3).resize(10);

	ASSERT_EQ(a.size(), 10);

	EXPECT_EQ(a[0], 1);
	EXPECT_EQ(a[1], 2);
	EXPECT_EQ(a[2], 3);
}

TEST(dynamic_vector_test, operators) {
	const aml::DVector<int> a(10, 20);
	const aml::DVector<int> b(-10, -20);

	int sc = 4;

	const auto a_plus_b = a + b;
	const aml::DVector<int> a_plus_b_ans(0, 0);
	EXPECT_EQ(a_plus_b, a_plus_b_ans);

	const auto a_minus_b = a - b;
	const aml::DVector<int> a_minus_b_ans(20, 40);
	EXPECT_EQ(a_minus_b, a_minus_b_ans);

	const auto a_mul_sc = a * sc;
	const aml::DVector<int> a_mul_sc_ans(40, 80);
	EXPECT_EQ(a_mul_sc, a_mul_sc_ans);

	const auto a_div_sc = a / sc;
	const aml::DVector<int> a_div_sc_ans(2, 5);
	EXPECT_EQ(a_div_sc, a_div_sc_ans);

	const auto sc_div_a = sc / a;
	const aml::DVector<int> sc_div_a_ans(0,0);
	EXPECT_EQ(sc_div_a, sc_div_a_ans);

	const auto minus_a = -a;
	const aml::DVector<int> minus_a_ans(-10, -20);
	EXPECT_EQ(minus_a, minus_a_ans);

	auto a_mut = a;

	a_mut += b;
	const aml::DVector<int> a_mut_plus_eq_b_ans(0, 0);
	EXPECT_EQ(a_mut, a_mut_plus_eq_b_ans);

	a_mut -= b;
	const aml::DVector<int> a_mut_minus_eq_b_ans(10, 20);
	EXPECT_EQ(a_mut, a_mut_minus_eq_b_ans);

	a_mut *= sc;
	const aml::DVector<int> a_mut_mul_eq_sc_ans(40, 80);
	EXPECT_EQ(a_mut, a_mut_mul_eq_sc_ans);

	a_mut /= sc;
	const aml::DVector<int> a_mut_div_eq_sc_ans(10, 20);
	EXPECT_EQ(a_mut, a_mut_div_eq_sc_ans);
}

TEST(dynamic_vector_test, functions) {
	const aml::DVector<int> a(1, 3, -2);
	const aml::DVector<int> b(10, 20, 0);

	const auto a_dot_b = aml::dot(a, b);
	EXPECT_EQ(a_dot_b, 70);

	const auto a_cross_b = aml::cross(a, b);
	const aml::DVector<int> a_cross_b_ans(40, -20, -10);
	EXPECT_EQ(a_cross_b, a_cross_b_ans);

	const auto normalize_a = aml::normalize(a);
	const aml::DVector<float> normalize_a_ans(0.267261237f, 0.801783681f, -0.534522474f);
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

TEST(dynamic_vector_test, cast_from_static_vector) 
{
	const aml::Vector<int, 3> a(10, 20, 30);
	const auto b = static_cast<aml::DVector<int>>(a);

	ASSERT_EQ(b.size(), 3);
	EXPECT_EQ(b[0], 10);
	EXPECT_EQ(b[1], 20);
	EXPECT_EQ(b[2], 30);
}

TEST(DynamicAndStaticVectorTest, operators)
{
	const aml::DVector<int> a(3, 7);

	static constexpr aml::Vector<int, 2> b(1, -4);

	const auto a_plus_b = a + b;
	const aml::DVector<int> a_plus_b_ans(4, 3);
	EXPECT_EQ(a_plus_b, a_plus_b_ans);

	const auto b_plus_a = b + a;
	const aml::DVector<int> b_plus_a_ans(4, 3);
	EXPECT_EQ(b_plus_a, b_plus_a_ans);

	const auto a_minus_b = a - b;
	const aml::DVector<int> a_minus_b_ans(2, 11);
	EXPECT_EQ(a_minus_b, a_minus_b_ans);

	const auto b_minus_a = b - a;
	const aml::DVector<int> b_minus_a_ans(-2, -11);
	EXPECT_EQ(b_minus_a, b_minus_a_ans);

	auto a_mut = a;

	a_mut += b;
	const aml::DVector<int> a_mut_plus_eq_b_ans(4, 3);
	EXPECT_EQ(a_mut, a_mut_plus_eq_b_ans);

	a_mut -= b;
	const aml::DVector<int> a_mut_minus_eq_b_ans(3, 7);
	EXPECT_EQ(a_mut, a_mut_minus_eq_b_ans);

	auto b_mut = b;

	b_mut += a;
	const aml::Vector<int, 2> b_mut_plus_eq_a_ans(4, 3);
	EXPECT_EQ(b_mut, b_mut_plus_eq_a_ans);

	b_mut -= a;
	const aml::Vector<int, 2> b_mut_minus_eq_a_ans(1, -4);
	EXPECT_EQ(b_mut, b_mut_minus_eq_a_ans);
}

TEST(DynamicAndStaticVectorTest, functions)
{
	const aml::DVector<int> a(2, -3, 4);
	static constexpr aml::Vector<int, 3> b(0, -1, 8);

	const auto a_dot_b = aml::dot(a, b);
	const auto a_dot_b_ans = 35;
	EXPECT_EQ(a_dot_b, a_dot_b_ans);

	const auto a_cross_b = aml::cross(a, b);
	const aml::DVector<int> a_cross_b_ans(-20, -16, -2);
	EXPECT_EQ(a_cross_b, a_cross_b_ans);

	const auto dist_between_a_b = aml::dist_between(a, b);
	const auto dist_between_a_b_ans = 4.89897919f;
	EXPECT_FLOAT_EQ(dist_between_a_b, dist_between_a_b_ans);
}

}
