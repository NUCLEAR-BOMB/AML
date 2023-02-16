
#include "Testing.hpp"

#include <AML/Fractions.hpp>

namespace {

DEFINE_TEST(fractions_init)
{
	DEFINE_VAR aml::Fraction<int> fract1(1, 2);
	
	TEST_EQUALS(fract1, 0.5);
	TEST_TRUE(fract1 > 0.2);
	TEST_TRUE(fract1 >= 0.5f);
	TEST_TRUE(fract1 < 1);
}

DEFINE_TEST(fractions_functions)
{
	DEFINE_VAR aml::Fraction fract1(7,4);
	DEFINE_VAR aml::Fraction fract2(15,8);

	TEST_EQUALS(aml::simplify(fract1), aml::Fraction(7,4));

	TEST_EQUALS(aml::simplify(fract2), fract2);

	FORCE_COMPILE_TIME({
		auto [fract12, fract22] = aml::common_denominator(fract1, fract2);

		if (fract12 != aml::Fraction(56,32)) throw 0;
		if (fract22 != aml::Fraction(60,32)) throw 0;

		if (fract12 != aml::Fraction(7,4)) throw 0;
		if (fract22 != aml::Fraction(15,8)) throw 0;
	});
}

DEFINE_TEST(fractions_operators)
{
	DEFINE_VAR aml::Fraction fractA(3,4);
	DEFINE_VAR aml::Fraction fractB(7,8);

	TEST_EQUALS(fractA + fractB,	aml::Fraction(13,8));
	TEST_EQUALS(fractB + fractA,	fractA + fractB);
	TEST_EQUALS(fractA + 1,			aml::Fraction(7,4));
	TEST_EQUALS(1 + fractA,			fractA + 1);

	TEST_EQUALS(fractA - fractB,	aml::Fraction(-1,8));
	TEST_EQUALS(fractB - fractA,	-(fractA - fractB));
	TEST_EQUALS(fractB - 1,			aml::Fraction(-1,8));
	TEST_EQUALS(1 - fractB,			-(fractB - 1));

	TEST_EQUALS(fractA * fractB,	aml::Fraction(21,32));
	TEST_EQUALS(fractB * fractA,	fractA * fractB);
	TEST_EQUALS(fractA * 2,			aml::Fraction(6,4));
	TEST_EQUALS(3 * fractB,			aml::Fraction(21,8));
	
	TEST_EQUALS(fractA / fractB,	aml::Fraction(24,28));
	TEST_EQUALS(fractB / fractA,	aml::Fraction(28,24));
	TEST_EQUALS(fractA / 2,			aml::Fraction(3,8));
	TEST_EQUALS(3 / fractB,			aml::Fraction(24,7));
	TEST_EQUALS(1 / fractA,			aml::Fraction(4,3));

	TEST_FALSE(fractA > fractB);
	TEST_TRUE(fractA >= fractA);
	TEST_FALSE(fractB < fractA);
	TEST_FALSE(fractB <= fractA);
	TEST_TRUE(fractA == aml::Fraction(6, 8));
	TEST_TRUE(fractA != fractB);
}

}
