
#include <AML/Tools.hpp>
#include <gtest/gtest.h>

#include <vector>
#include <list>

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

static constexpr
void test_of_bytes() {
	using namespace std;
	static_assert(is_same_v<std::make_unsigned_t<signed_from_bytes<0>>, unsigned_from_bytes<0>>);

	static_assert(is_same_v<signed_from_bytes<0>, int8_t>);
	static_assert(is_same_v<signed_from_bytes<1>, int8_t>);
	static_assert(is_same_v<signed_from_bytes<2>, int16_t>);
	static_assert(is_same_v<signed_from_bytes<3>, int32_t>);
	static_assert(is_same_v<signed_from_bytes<4>, int32_t>);
	static_assert(is_same_v<signed_from_bytes<5>, int64_t>);
	static_assert(is_same_v<signed_from_bytes<6>, int64_t>);
	static_assert(is_same_v<signed_from_bytes<7>, int64_t>);
	static_assert(is_same_v<signed_from_bytes<8>, int64_t>);

	static_assert(0 > sizeof(float) || is_same_v<floating_point_from_bytes<0>, float>);
	static_assert(1 > sizeof(float) || is_same_v<floating_point_from_bytes<1>, float>);
	static_assert(2 > sizeof(float) || is_same_v<floating_point_from_bytes<2>, float>);
	static_assert(3 > sizeof(float) || is_same_v<floating_point_from_bytes<3>, float>);
	static_assert(4 > sizeof(float) || is_same_v<floating_point_from_bytes<4>, float>);
}

static constexpr
void test_of_bits() {
	using namespace std;
	static_assert(is_same_v<std::make_unsigned_t<signed_from_bits<0>>, unsigned_from_bits<0>>);

	static_assert(is_same_v<signed_from_bits<0>, int8_t>);
	static_assert(is_same_v<signed_from_bits<8>, int8_t>);
	static_assert(is_same_v<signed_from_bits<9>, int16_t>);
	static_assert(is_same_v<signed_from_bits<16>, int16_t>);
	static_assert(is_same_v<signed_from_bits<17>, int32_t>);
	static_assert(is_same_v<signed_from_bits<32>, int32_t>);
	static_assert(is_same_v<signed_from_bits<33>, int64_t>);
	static_assert(is_same_v<signed_from_bits<64>, int64_t>);

	static_assert(0 > sizeof(float) || is_same_v<floating_point_from_bits<0>, float>);
	static_assert(1 > sizeof(float) || is_same_v<floating_point_from_bits<15>, float>);
	static_assert(2 > sizeof(float) || is_same_v<floating_point_from_bits<17>, float>);
	static_assert(3 > sizeof(float) || is_same_v<floating_point_from_bits<31>, float>);
	static_assert(4 > sizeof(float) || is_same_v<floating_point_from_bits<32>, float>);
}

TEST(ToolsOther, custom_type) {
	ASSERT_EQ((int)aml::zero, 0);
	ASSERT_EQ((int)aml::one, 1);
}

}
