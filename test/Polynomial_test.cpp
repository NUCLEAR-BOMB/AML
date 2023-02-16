
#include "Testing.hpp"

#include <AML/Polynomial.hpp>

namespace {

DEFINE_TEST(polynomial_init)
{
	DEFINE_VAR aml::Polynomial<int, 1> poly1(1, 2);
	DEFINE_VAR aml::Polynomial<float, 2> poly2(1.2f, 10.f, 1.f);
}

DEFINE_TEST(polynomial_operators)
{
	DEFINE_VAR aml::Polynomial polyA(-1.25f, 2.f);
	//DEFINE_VAR aml::Polynomial polyB(0, 10);

	TEST_EQUALS((polyA * 2), aml::Polynomial(-2.5f, 4.f));
	TEST_EQUALS((2 * polyA), (polyA * 2));

	TEST_EQUALS((polyA / 2), aml::Polynomial(-0.625f, 1.f));
	TEST_EQUALS((2 / polyA), aml::Polynomial(-1.6f, 1.f));
}

DEFINE_TEST(polynomial_monic)
{
	DEFINE_VAR aml::Polynomial poly1(3, 6, 3);

	TEST_EQUALS(aml::monic(poly1), aml::Polynomial(1, 2, 1));
}

DEFINE_TEST(polynomial_degree_1)
{
	TEST_EQUALS(aml::solve(aml::Polynomial(200, 100)), -2.f);
	TEST_EQUALS(aml::solve(aml::Polynomial(1.f, 2.f)), -0.5f);
}

DEFINE_TEST(polynomial_degree_2)
{
	TEST_EQUALS(aml::solve(aml::Polynomial(4, 8, 4)), -1.f);
	TEST_EQUALS(aml::solve_polynomial(4, 8, 4), -1.f);
}

}
