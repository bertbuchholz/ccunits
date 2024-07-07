#include <cmath>

#include <gtest/gtest.h>

#include "Units.h"

#include "test_utils.h"

// User defined Quantity (jerk?)
using MyQuantity = ccunits::Quantity<
    std::tuple<ccunits::base::Length>,
    std::tuple<ccunits::base::Duration, ccunits::base::Duration, ccunits::base::Duration>>;

// User defined Unit
struct BaseUnitForMyQuantity : ccunits::Unit<MyQuantity, std::ratio<1>> {};
struct KiloUnitForMyQuantity : ccunits::Unit<MyQuantity, std::kilo> {};
struct FloatUnitForMyQuantity : ccunits::Unit<MyQuantity, ccunits::ratiod<1.5, 2.5>> {};

// Could also use DEFINE_UNITS if the units are regular (micro, milli, kilo, mega etc.) and the base unit has no prefix
// (unlike kilogram).

DEFINE_LITERAL(MyQuantity, BaseUnitForMyQuantity, bumq)

TEST(UserDefinition, UseQuantity) {
    auto m = MyQuantity::from<BaseUnitForMyQuantity>(4.0);
    EXPECT_EQ(m.to<BaseUnitForMyQuantity>(), 4.0);
}

TEST(UserDefinition, UseConversion) {
    auto m = MyQuantity::from<KiloUnitForMyQuantity>(4.0);
    EXPECT_EQ(m.to<BaseUnitForMyQuantity>(), 4000.0);

    EXPECT_NEAR(m.to<FloatUnitForMyQuantity>(), 4000.0 * (2.5 / 1.5), 1e-5);
}

TEST(UserDefinition, UseLiteral) {
    using namespace literals;
    auto m = 4.0_bumq;
    EXPECT_EQ(m.to<BaseUnitForMyQuantity>(), 4.0);
}

TEST(UserDefinition, UseWithOtherQuantities) {
    using namespace ccunits::literals;

    auto m = MyQuantity::from<KiloUnitForMyQuantity>(4.0);
    auto t = 3_s;
    auto s = m * t * t * t;
    EXPECT_EQ(s.to<ccunits::Meter>(), 4000.0 * 3.0 * 3.0 * 3.0);
}
