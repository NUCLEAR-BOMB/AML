#include "Testing.hpp"

#include <AML/Expressions.hpp>

namespace {

DEFINE_TEST(expressions) 
{
	DEFINE_VAR aml::Expression expr1("1.5 * 2 + (1/2)*(2/1) - 1");
	DEFINE_VAR auto result1 = expr1.calculate();
	TEST_EQUALS(result1, 3.0);

	DEFINE_VAR auto result2 = aml::Expression("30 / 2 * 1 - 1").calculate();
	TEST_EQUALS(result2, 14.0);
}

}

