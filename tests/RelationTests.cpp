#include <cmath>

#include <gtest/gtest.h>

#include "Units.h"

#include "test_utils.h"

using namespace ccunits;
using namespace ccunits::literals;

TEST(Relation, Distance_Speed_Acceleration) {
    {
        Velocity const s = 1_km / 1_hr;

        EXPECT_EQ(s, 1_kmph);
        EXPECT_EQ(s * 60_min, 1_km);
    }

    {
        // s = s0 + v0 * t + a0 * t^2
        // s = 5m + 2m/s * 3s + 5m/s^2 * (3s)^2 = 56m
        auto const s0 = 5_m;
        auto const v0 = 2_mps;
        auto const t = 3_s;
        auto const a0 = 5_mps / 1_s;
        auto const s = s0 + v0 * t + a0 * t * t;

        EXPECT_EQ(s, 56_m);
    }
}

TEST(Relation, Electricity_CoulombsLaw) {
    // F_C = 1 / (4 pi epsilon_0) * (q_1 * q_2) / r^2
    // epsilon_0: Farad/m = C/(V*m)
    // q: Charge (Coulomb)
    // r: Distance (m)

    auto constexpr epsilon_0 = 8.8541878176e-12_F / 1_m;
    auto constexpr r = 1_m;
    auto constexpr q_1 = 1_C;
    auto constexpr q_2 = 1_C;
    Force constexpr F_C = 1.0 / (4.0 * M_PI * epsilon_0) * q_1 * q_2 / (r * r);

    auto constexpr k = 1.0f / (4.0f * M_PI * 8.8541878176e-12);
    EXPECT_NEAR(F_C.to<Newton>(), k, 1e-15) << F_C.to<Newton>() << " " << k;
    static_assert(math::abs(F_C - Force::from<Newton>(k)) < 1e-15_N);
}

TEST(Relation, Electricity_CoulombsLaw_DifferentOrders) {
    auto const epsilon_0 = 8.8541878176e-12_F / 1_m;
    auto const r = 1_m;
    auto const q_1 = 1_C;
    auto const q_2 = 1_C;
    Force const F_C1 = 1.0 / (4.0 * M_PI * epsilon_0) * q_1 * q_2 / (r * r);
    Force const F_C2 = q_2 / (r * r) * q_1 / (4.0 * M_PI * epsilon_0);
    Force const F_C3 = (1.0 / r) * q_1 / (4.0 * M_PI * epsilon_0) * q_2 / r;

    EXPECT_EQ(F_C1, F_C2);
    EXPECT_EQ(F_C1, F_C3);
}

TEST(Relation, Pressure_ForceOverArea) {
    EXPECT_TRUE(isNear(5_N / 10_cm2, 5000_Pa, 0.01_Pa));
}
