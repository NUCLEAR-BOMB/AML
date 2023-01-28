
#include <AML/Polynomial.hpp>

#include <gtest/gtest.h>

using namespace aml;

namespace {


TEST(PolynomialTest, variadic_init)
{
	Polynomial<int, 1> a(1);
	Polynomial<int, 2> b(1, 2);
	Polynomial<float, 3> c(1.2f, 3.f, 10.f);
}

TEST(PolynomialTest, degree_0)
{
	const Polynomial<int, 1> a(10);

	const auto a_res = aml::solve(a);
	ASSERT_EQ(a_res, 0);

	const auto b_res = aml::solve(Polynomial(-123243));
	ASSERT_EQ(b_res, 0);
}

TEST(PolynomialTest, degree_1)
{
	const Polynomial<float, 2> a(200.f, 100.f);

	const auto a_res = aml::solve(a);
	ASSERT_FLOAT_EQ(a_res, -2.f);

	const auto b_res = aml::solve(Polynomial(1.f, 2.f));

	ASSERT_FLOAT_EQ(b_res, -0.5f);
}

TEST(PolynomialTest, degree_2)
{
	const Polynomial a(1.f, 5.f, 3.f);

	const auto a_res = aml::solve(a);
	ASSERT_FLOAT_EQ(a_res, -0.23240812f);

	const auto b_res = aml::solve(aml::Polynomial(4.f, 8.f, 4.f));
	ASSERT_FLOAT_EQ(b_res, -1.f);

	const auto c_res = aml::solve(aml::Polynomial(1.f, 2.f, 3.f));

	ASSERT_TRUE(c_res.has_roots());
	ASSERT_FALSE(c_res.has_real_roots());

	ASSERT_EQ(aml::get<0>(c_res), aml::Complex(-0.33333333f, 0.47140452f));
}


}