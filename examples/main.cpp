#include <chrono>
#include <iostream>

#include "Units.h"

using namespace ccunits;
using namespace ccunits::literals;

using namespace std::chrono_literals;

int main() {
    // Basic usage (Length)
    constexpr float someLengthInKilometers = 0.001f;

    // Convert from "unsafe" raw value to a quantity
    // The quantity only needs to know the unit of the input, but the original unit is no longer
    // relevant after creation of the quantity
    constexpr auto anotherLength = Length::from<Kilometer>(someLengthInKilometers);

    // _value is storing the quantity in the base unit (ratio: 1). It shouldn't be used directly.
    std::cout << "Direct _value output for 0.001 km: " << anotherLength._value << "\n";

    // Instead .to<>() should've used.
    constexpr float anotherLengthInMeters = anotherLength.to<Millimeter>();

    std::cout << "Length: 1m in mm: " << anotherLengthInMeters << "\n";

    // static_assert
    static_assert(IsQuantity<int> == false);
    static_assert(SameQuantity<Length, Duration> == false);

    static_assert(1_km == 1000_m);
    static_assert(Length::from<Mile>(1) == 1609.344_m);

    // Area
    constexpr auto area = Length::from<Meter>(1) * Length::from<Millimeter>(1);
    std::cout << "Area explicit: " << area.to<SquareMeter>() << "\n";

    constexpr auto area2 = 1_m * 1_mm;
    std::cout << "Area with literals: " << area2.to<SquareMeter>() << "\n";

    std::cout << "Area 1m * 1mm in mm^2: " << area.to<SquareMillimeter>() << "\n";

    // Duration (and std::chrono)
    // Automatic conversion from std::chrono::duration to Duration
    constexpr Duration durationFromChrono1 = 1s;
    std::cout << "1s from chrono in seconds: " << durationFromChrono1.to<Second>() << "\n";

    constexpr Duration durationFromChrono2 = 1h;
    std::cout << "1h from chrono in seconds: " << durationFromChrono2.to<Second>() << "\n";

    // In expressions, Duration needs to be explicitly constructed from std::chrono::duration
    constexpr auto speedWithChrono = Length::from<Kilometer>(1) / Duration(1h);
    std::cout << "1 kmph with chrono to mps: " << speedWithChrono.to<MeterPerSecond>() << "\n";

    // Duration and Timepoint

    // Not possible to add Timepoints:
    // auto t1 = Timepoint::from<Second>(5) + Timepoint::from<Second>(5);
    // A Timepoint is not a Duration:
    // Timepoint t2 = Duration::from<Second>(5);

    constexpr auto speed = Length::from<Kilometer>(1.0) / 1.0_s;
    std::cout << "1 kmps to mps: " << speed.to<MeterPerSecond>() << "\n";
    std::cout << "1 kmps to kmph: " << speed.to<KilometerPerHour>() << "\n";

    std::cout << "10mps * 5s: " << (10_mps * 5.0_s).to<Meter>() << "\n";

    constexpr auto speedOfLight = Velocity::from<MeterPerSecond>(299'792'458);
    std::cout << "Speed of light * 1ns in cm: " << (speedOfLight * 1.0_ns).to<Centimeter>() << " cm\n";

    std::cout << "Length 30.4m in km: " << (30.4_m).to<Kilometer>() << "\n";
    std::cout << "Length 30km in km: " << (30_km).to<Kilometer>() << "\n";

    // Trigonometry
    constexpr Angle pi = Angle::from<Radian>(M_PI);

    std::cout << "45 deg in radians: " << (45.0_deg).to<Radian>() << "\n";
    std::cout << "90 deg in radians: " << (90.0_deg).to<Radian>() << "\n";
    std::cout << "180 deg in radians: " << (180.0_deg).to<Radian>() << "\n";
    std::cout << "pi rad in degrees: " << (3.14159_rad).to<Degree>() << "\n";
    std::cout << "pi in rad: " << (pi).to<Radian>() << "\n";
    std::cout << "pi + 90 deg in degrees: " << (pi + 90.0_deg).to<Degree>() << "\n";

    std::cout << "Angle cos(M_PI * 0.5) = 0: " << std::cos(M_PI_2) << "\n";
    std::cout << "Angle cos(90 deg): = 0: " << ccunits::math::cos(90.0_deg) << "\n";
    std::cout << "Angle acos(0) = 90 deg: " << ccunits::math::acos(0).to<Degree>() << "\n";

    std::cout << "Angle atan2(1, 1) = 45 deg: " << ccunits::math::atan2(1, 1).to<Degree>() << "\n";
    std::cout << "Angle atan2(1, 0) = 90 deg: " << ccunits::math::atan2(1, 0).to<Degree>() << "\n";

    // Temperature
    constexpr Temperature temp = Temperature::from<Kelvin>(5.0);
    std::cout << "Temperature: " << temp.to<Celsius>() << "\n";
    std::cout << "Temperature: 5K in Celsius: " << (5.0_K).to<Celsius>() << "\n";
    std::cout << "Temperature: 5K in Kelvin: " << (5.0_K).to<Kelvin>() << "\n";
    //std::cout << "Temp 20C in Celsius: " << Temperature::to<Celsius>(20.0_C) << "\n";
    //std::cout << "Temp 20C in Kelvin: " << Temperature::to<Kelvin>(20.0_C) << "\n";

    constexpr Temperature t1 = 0_K;

    // Not possible to express temperatures via the C literal:
    // static_assert(t1 == -273.15_C);
    // Need to use explicit creation:
    static_assert(t1 == Temperature::from<Celsius>(-273.15));
    std::cout << "Temperature 0K in C: " << t1.to<Celsius>() << "\n";

    constexpr Temperature t2 = Temperature::from<Celsius>(20);

    static_assert(t2 == Temperature::from<Celsius>(20));
    static_assert(t2 == 293.15_K);

    std::cout << "Temperature 293.15K in C: " << t2.to<Celsius>() << "\n";

    // Adding a temperature delta (corresponding to duration vs timepoint)
    constexpr auto t3 = t2 + 20_dC;
    static_assert(t3 == Temperature::from<Celsius>(40));

    // The difference between two temperatures is a temperature delta
    static_assert(10_dK == 30_K - 20_K);
    // It's not another temperature:
    // static_assert(10_K == 30_K - 20_K);

    // Electricity
    // U = R * I
    Voltage v1 = Resistance::from<Ohm>(2) * Current::from<Ampere>(5);
    std::cout << "Voltage: 2 Ohm * 5A = 10V: " << v1.to<Volt>() << "\n";

    Current c1 = Voltage::from<Volt>(2) / Resistance::from<Ohm>(2);
    std::cout << "Current: 2V / 2 Ohm = 1A: " << c1.to<Ampere>() << "\n";

    // P = U * I
    Power p1 = Voltage::from<Volt>(2) * Current::from<Ampere>(4);
    std::cout << "Power: 2V * 2 A = 8 W: " << p1.to<Watt>() << "\n";

    // Miscellaneous
    std::cout << "Acceleration 5mps / 1s = 5m/s^2: " << (5.0_mps / 1.0_s).to<MeterPerSquareSecond>() << "\n";

    std::cout << "Force 20kg * 5mps/s = 100N: " << (20.0_kg * 5.0_mps / 1.0_s).to<Newton>() << "\n";

    std::cout << "Energy 10N * 5m = 50J: " << (10.0_N * 5.0_m).to<Joule>() << "\n";

    std::cout << "Energy 10W * 1s = 10Ws: " << (10.0_W * 1.0_s).to<WattSecond>() << "\n";

    std::cout << "Power 10Ws / 5s = 2W: " << (10.0_W * 1.0_s / 5.0_s).to<Watt>() << "\n";
    std::cout << "Power 10kWh / 5h = 2kW: " << (10.0_W * 1000.0f * 1.0_hr / 5.0_hr).to<Kilowatt>() << "\n";

    std::cout << "Volume 5m^3 / 2m = 2.5m^2: " << (5_m3 / 2_m).to<SquareMeter>() << "\n";
    std::cout << "Volume 5m^3 / 2m^2 = 2.5m: " << (5_m3 / 2_m2).to<Meter>() << "\n";

    std::cout << "Pressure 1bar in mbar: " << (1_bar).to<Millibar>() << "\n";
    std::cout << "Pressure 1bar in hectopascal: " << (1_bar).to<Hectopascal>() << "\n";
    std::cout << "Pressure 5N / 10cm^2 = 5000 Pa: " << (5_N / 10_cm2).to<Pascal>() << "\n";

    // Comparisons
    std::cout << "1km < 2000m = true: " << (1_km < 2000_m ? "true" : "false") << "\n";
    static_assert(1_km < 2000_m);

    // Double comparison, not stable
    std::cout << "36m/h == 10m/s = true: " << (36_kmph == 10_mps ? "true" : "false") << "\n";
    std::cout << "37m/h == 10m/s = false: " << (37_kmph == 10_mps ? "true" : "false") << "\n";

    std::cout << "operator<< with 5_km: " << 5_km << "\n";
    std::cout << "sizeof(Length): " << sizeof(Length) << "\n";

    // Only the value is stored, no storage overhead
    static_assert(sizeof(Length) == sizeof(Length::Rep));
}
