#include <gtest/gtest.h>

#include "Units.h"

using namespace ccunits;
using namespace ccunits::literals;

bool isNear(Angle const& a1, Angle const& a2, double const epsilon) {
    return std::abs(Angle::to<Radian>(a1 - a2)) < epsilon;
}

TEST(Math, Angle_TrigonometricFunctions) {
    EXPECT_NEAR(ccunits::math::cos(90_deg), 0.0, 1e-10);
    EXPECT_NEAR(ccunits::math::sin(90_deg), 1.0, 1e-10);
    EXPECT_NEAR(
        ccunits::math::sin(90_deg),
        ccunits::math::sin(Angle::from<Radian>(ccunits::constants::pi) * 0.5),
        1e-10);

    EXPECT_NEAR(ccunits::math::tan(45_deg), 1.0, 1e-10);

    EXPECT_EQ(ccunits::math::asin(0.0), 0_deg);
    EXPECT_EQ(ccunits::math::asin(0.0), 0_rad);
    EXPECT_TRUE(isNear(ccunits::math::acos(0.0), 90_deg, 1e-10));

    EXPECT_TRUE(isNear(ccunits::math::atan2(1, 1), 45_deg, 1e-10));
    EXPECT_TRUE(isNear(ccunits::math::atan2(1, 0), 90_deg, 1e-10));
}