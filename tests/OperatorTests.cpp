#include <gtest/gtest.h>

#include "Units.h"

using namespace ccunits;
using namespace ccunits::literals;

TEST(Operators, Negative) {
    constexpr Length l1 = 1_m;
    constexpr Length l2 = -1_m;

    EXPECT_EQ(-l1, l2);
    EXPECT_EQ(l1, -1 * l2);
}

TEST(Operators, Comparison) {
    constexpr Length l1 = 1_m;
    constexpr Length l2 = 2_m;
    constexpr Length l3 = 2_m;

    EXPECT_NE(l1, l2);
    EXPECT_LT(l1, l2);
    EXPECT_GT(l2, l1);
    EXPECT_EQ(l2, l3);
}

TEST(Operators, Addition) {
    constexpr Length l1 = 1_m;
    constexpr Length l2 = 2_m;

    EXPECT_EQ(Length::to<Meter>(l1 + l2), 3.0);
    EXPECT_EQ(l1 + l2, 3_m);
    EXPECT_EQ(l1 + l2, l2 + l1);
    EXPECT_EQ(1_m + 2_m, 3_m);
}

TEST(Operators, Subtraction) {
    constexpr Length l1 = 1_m;
    constexpr Length l2 = 2_m;

    EXPECT_EQ(Length::to<Meter>(l2 - l1), 1.0);
    EXPECT_EQ(Length::to<Meter>(l1 - l2), -1.0);
    EXPECT_EQ(l1 - l2, -1_m);
    EXPECT_EQ(2_m - 1_m, 1_m);
}

TEST(Operators, ScalarMultiplication) {
    constexpr Length l1 = 2_m;

    EXPECT_EQ(Length::to<Meter>(5.0 * l1), 10.0);
    EXPECT_EQ(Length::to<Meter>(l1 * 5.0), 10.0);
    EXPECT_EQ(Length::to<Meter>(l1 * -5.0), -10.0);
    EXPECT_EQ(Length::to<Meter>(-l1 * -5.0), 10.0);
    EXPECT_EQ(l1 * 5.0, 10.0_m);
    EXPECT_EQ(3_m * 5.0, 15.0_m);
}

TEST(Operators, ScalarDivision) {
    constexpr Length l1 = 2_m;

    EXPECT_EQ(Length::to<Meter>(l1 / 2.5), 0.8);

    // Not commutative
    // EXPECT_EQ(Length::to<Meter>(2.5 / l1), -);

    EXPECT_EQ(Length::to<Meter>(l1 / -5.0), -0.4);

    EXPECT_EQ(l1 / 5.0, 0.4_m);
    EXPECT_EQ(3_m / 5.0, 0.6_m);
}

TEST(Operators, SelfDivision) {
    constexpr Length l1 = 1_m;
    constexpr Length l2 = 2_m;

    EXPECT_EQ(l1 / l2, 0.5);
}