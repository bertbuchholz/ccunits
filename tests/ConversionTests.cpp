#include <gtest/gtest.h>

#include "Units.h"

using namespace ccunits;
using namespace ccunits::literals;

TEST(Conversion, Temperature) {
    const auto zeroKelvinInCelsius = Temperature::from<Celsius>(-273.15);
    EXPECT_EQ(0_K, zeroKelvinInCelsius);

    EXPECT_EQ(273.15_K, Temperature::from<Celsius>(0));

    // Large values convert properly
    EXPECT_EQ(1'000'000_K, 1'000'273.15_K - TemperatureDifference::from<dCelsius>(273.15));
    EXPECT_EQ(Temperature::from<Celsius>(1'000'000), 1'000'273.15_K);

    // Doesn't compile, can't add two temperature points
    // EXPECT_EQ(20_K, 10_K + 10_K);
}

TEST(Conversion, TemperatureDifference) {
    constexpr auto temp10C = Temperature::from<Celsius>(10);
    constexpr auto temp20C = Temperature::from<Celsius>(20);

    constexpr auto tempDiff10C = TemperatureDifference::from<dCelsius>(10);
    constexpr auto tempDiff20K = TemperatureDifference::from<dKelvin>(20);

    EXPECT_EQ(TemperatureDifference::from<dKelvin>(30), tempDiff10C + tempDiff20K);
    EXPECT_EQ(TemperatureDifference::from<dKelvin>(-10), tempDiff10C - tempDiff20K);

    EXPECT_EQ(TemperatureDifference::from<dKelvin>(10), temp20C - temp10C);
    EXPECT_EQ(TemperatureDifference::from<dKelvin>(-10), temp10C - temp20C);

    EXPECT_EQ(10_K, 0_K + TemperatureDifference::from<dKelvin>(10));
    EXPECT_EQ(10_K, 0_K + TemperatureDifference::from<dCelsius>(10));

    EXPECT_EQ(10_dC, TemperatureDifference::from<dCelsius>(10));
    EXPECT_EQ(10_dK, TemperatureDifference::from<dKelvin>(10));
    EXPECT_EQ(10_dK, TemperatureDifference::from<dCelsius>(10));
}

TEST(Conversion, Power) {
    EXPECT_EQ(1'000'000_uW, 1_W);
    EXPECT_EQ(1_GW, 1'000'000'000_W);
}

TEST(Conversion, Energy) {
    EXPECT_EQ(1_J, 1_Ws);
    EXPECT_EQ(1_Wh, 3600_Ws);
    EXPECT_EQ(5_kWh, 5000_Wh);
}

TEST(Conversion, Pressure) {
    EXPECT_EQ(1_bar, 1'000_mbar);
    EXPECT_EQ(1_bar, 100000_Pa);
}

TEST(Conversion, Speed) {
    EXPECT_EQ(60_kmph, 16_mps + 2.0_mps / 3.0); // 16.66666...
    EXPECT_EQ(1_mps, 3.6_kmph);
}

TEST(Conversion, Area) {
    EXPECT_EQ(1'000'000_mm2, 1_m2);
    EXPECT_EQ(1_cm2, 0.0001_m2);
    EXPECT_EQ(1_km2, 1'000'000_m2);
}
