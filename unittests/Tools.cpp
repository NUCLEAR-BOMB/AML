
#include <AML/Tools.hpp>
#include <gtest/gtest.h>

using namespace aml;

namespace {

TEST(static_for_test, no_arg)
{
	int a = 0;
	static_for<0, 10>([&] {
		a += 2;
	});
	ASSERT_EQ(a, 20);
}

TEST(static_for_test, with_arg)
{
	int a = 0;
	static_for<0, 10>([&](auto i) {
		a += i;
	});
	ASSERT_EQ(a, 45);
}

TEST(static_for_test, zero_to_range_no_arg)
{
	int a = 0;
	static_for<10>([&] {
		a += 3;
	});
	ASSERT_EQ(a, 30);
}

TEST(static_for_test, zero_to_range_with_arg)
{
	int a = 0;
	static_for<10>([&](auto i) {
		a += i * 2;
	});
	ASSERT_EQ(a, 90);
}

TEST(static_for_test, force_max)
{
	int a = 3;
	ASSERT_EQ(STATIC_FOR_MAX, 30);

	static_for<0, 40>([&]() {
		a += 3;
	});
	ASSERT_EQ(a, 123);
}



}
