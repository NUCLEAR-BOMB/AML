
#include "Testing.hpp"

#include <AML/Containers.hpp>

namespace {

DEFINE_TEST(fixed_valarray_init)
{
	DEFINE_VAR aml::fixed_valarray arr1({1, 2, 3, 4});
	TEST_EQUALS(arr1[0], 1);
	TEST_EQUALS(arr1[1], 2);
	TEST_EQUALS(arr1, aml::fixed_valarray({1, 2, 3, 4}));

	DEFINE_VAR aml::fixed_valarray<int, 10> arr2(aml::zero);
	TEST_EQUALS(arr2[0], 0);
	TEST_EQUALS(arr2[1], 0);
	TEST_EQUALS(arr2, aml::fixed_valarray({0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));

	DEFINE_VAR aml::fixed_valarray<int, 2> arr3(aml::one);
	TEST_EQUALS(arr3[0], 1);
	TEST_EQUALS(arr3[1], 1);
	TEST_EQUALS(arr3, aml::fixed_valarray({1, 1}));
}

DEFINE_TEST(fixed_valarray_operators)
{
	DEFINE_VAR aml::fixed_valarray arrA({1, 4});
	DEFINE_VAR aml::fixed_valarray arrB({0.f, -1.f});

	TEST_EQUALS((arrA + arrB), aml::fixed_valarray({1, 3}));
	TEST_EQUALS((arrB + arrA), (arrA + arrB));
	
	TEST_EQUALS((arrA - arrB), aml::fixed_valarray({1, 5}));
	TEST_EQUALS((arrB - arrA), -(arrA - arrB));

	TEST_EQUALS((arrA * arrB), aml::fixed_valarray({0, -4}));
	TEST_EQUALS((arrB * arrA), (arrA * arrB));

	TEST_EQUALS((arrA * -1.5f), aml::fixed_valarray({-1.5f, -6.f}));
	TEST_EQUALS((-1.5f * arrA), (arrA * -1.5f));

	TEST_EQUALS((arrB / arrA), aml::fixed_valarray({0.f, -1/4.f}));

	TEST_EQUALS((arrA / 2.f), aml::fixed_valarray({1/2.f, 2.f}));
	TEST_EQUALS((2.f / arrA), (1 / (arrA / 2.f)));
}

}
