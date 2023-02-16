#pragma once

#include <AML/Functions.hpp>

//#define NO_TESTS

#define DEFINE_TEST(test_name) \
	[[maybe_unused]] constexpr void test_name()

#ifndef NO_TESTS
	#define DEFINE_VAR \
		constexpr
#else
	#define DEFINE_VAR \
		[[maybe_unused]] constexpr
#endif

#ifndef NO_TESTS
	#define TEST_EQUALS(actual, expected) \
		static_assert(aml::equal(expected, actual), "Expected: \"" #expected "\" from \"" #actual "\"")
#else
	#define TEST_EQUALS(actual, expected) ((void)0)
#endif

#ifndef NO_TESTS
	#define TEST_TRUE(actual) \
		static_assert(actual, "Expected true: \"" #actual "\"")
#else
	#define TEST_TRUE(actual) ((void)0)
#endif
#ifndef NO_TESTS
	#define TEST_FALSE(actual) \
		static_assert(!(actual), "Expected false: \"" #actual "\"")
#else
	#define TEST_FALSE(actual) ((void)0)
#endif




#define FORCE_COMPILE_TIME(...) \
	static_assert((([&]() __VA_ARGS__ ()), void(), true))

