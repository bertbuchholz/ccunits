#include <gtest/gtest.h>

#include "Units.h"

using namespace ccunits;
using namespace ccunits::literals;

TEST(Conversion, Temperature) {
    const auto zeroKelvinInCelsius = Temperature::from<Celsius>(-273.15);
    EXPECT_EQ(0_K, zeroKelvinInCelsius);

    EXPECT_EQ(273.15_K, Temperature::from<Celsius>(0));

    // Doesn't compile, can't add two temperature points
    // EXPECT_EQ(20_K, 10_K + 10_K);
}

TEST(Conversion, TemperatureDifference) {
    constexpr auto temp10C = Temperature::from<Celsius>(10);
    constexpr auto temp20C = Temperature::from<Celsius>(20);

    constexpr auto tempDiff10C = TemperatureDifference::from<dCelsius>(10);
    constexpr auto tempDiff20K = TemperatureDifference::from<dKelvin>(20);

    const auto zeroKelvinInCelsius = Temperature::from<Celsius>(-273.15);
    EXPECT_TRUE(0_K == zeroKelvinInCelsius);

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
