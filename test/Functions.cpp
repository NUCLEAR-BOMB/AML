
#include "Testing.hpp"

#include <AML/Functions.hpp>

namespace {

DEFINE_TEST(functions_max)
{
	TEST_EQUALS(aml::max(2, 1), 2);
	TEST_EQUALS(aml::max(1, -2, 3), 3);
	TEST_EQUALS(aml::max(0, 0.f, 1.0), 1.0);

	FORCE_COMPILE_TIME({
		int a = 100;
		int b = 200;
		aml::max(a, b) = 1;

		if (b != 1) throw 0;
	});

	DEFINE_VAR auto num1 = -100;
	DEFINE_VAR auto num2 = 1;

	TEST_EQUALS(aml::max(num1, num2), 1);
}

DEFINE_TEST(functions_min)
{
	TEST_EQUALS(aml::min(2, 1), 1);
	TEST_EQUALS(aml::min(1, -2, 3), -2);
	TEST_EQUALS(aml::min(0, 0.f, 1.0), 0.0);
}

}
