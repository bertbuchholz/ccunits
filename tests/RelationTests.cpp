#include <gtest/gtest.h>

#include "Units.h"

using namespace ccunits;
using namespace ccunits::literals;

TEST(Relation, Speed) {
    Speed const s = 1_km / 1_hr;

    EXPECT_EQ(s, 1_kmph);
    EXPECT_EQ(s * 60_min, 1_km);
}