#include <iostream>

#include "Units.h"

using namespace ccunits;
using namespace ccunits::literals;

int main() {
    constexpr float someLengthInKilometers = 0.001f;

    constexpr auto anotherLength = Length::from<Kilometer>(someLengthInKilometers);

    std::cout << "Direct _value output for 0.001 km: " << anotherLength._value << "\n";

    constexpr float anotherLengthInMeters = Length::to<Millimeter>(anotherLength);

    std::cout << "Length: 1m in mm: " << anotherLengthInMeters << "\n";

    constexpr auto area = Length::from<Meter>(1) * Length::from<Millimeter>(1);
    std::cout << "Area explicit: " << Area::to<SquareMeter>(area) << "\n";

    constexpr auto area2 = 1_m * 1_mm;
    std::cout << "Area with literals: " << Area::to<SquareMeter>(area2) << "\n";

    std::cout << "Area 1m * 1mm in mm^2: " << Area::to<SquareMillimeter>(area) << "\n";

    // Automatic conversion from std::chrono::duration to Duration
    constexpr Duration durationFromChrono1 = 1s;
    std::cout << "1s from chrono in seconds: " << Duration::to<Second>(durationFromChrono1) << "\n";

    constexpr Duration durationFromChrono2 = 1h;
    std::cout << "1h from chrono in seconds: " << Duration::to<Second>(durationFromChrono2) << "\n";

    constexpr auto speedWithChrono = Length::from<Kilometer>(1) / 1h;
    std::cout << "1 kmph with chrono to mps: " << Speed::to<MetersPerSecond>(speedWithChrono) << "\n";

    constexpr auto speed = Length::from<Kilometer>(1) / 1.0_s;
    std::cout << "1 kmps to mps: " << Speed::to<MetersPerSecond>(speed) << "\n";
    std::cout << "1 kmps to kmph: " << Speed::to<KilometersPerHour>(speed) << "\n";

    std::cout << "10mps * 5s: " << Length::to<Meter>(10_mps * 5.0_s) << "\n";

    constexpr auto speedOfLight = Speed::from<MetersPerSecond>(299'792'458);
    std::cout << "Speed of light * 1ns in cm: " << Length::to<Centimeter>(speedOfLight * 1.0_ns) << " cm\n";

    std::cout << "Length 30.4m in km: " << Length::to<Kilometer>(30.4_m) << "\n";
    std::cout << "Length 30km in km: " << Length::to<Kilometer>(30_km) << "\n";

    constexpr Angle pi = Angle::from<Radian>(ccunits::constants::pi);

    std::cout << "45 deg in radians: " << Angle::to<Radian>(45.0_deg) << "\n";
    std::cout << "90 deg in radians: " << Angle::to<Radian>(90.0_deg) << "\n";
    std::cout << "180 deg in radians: " << Angle::to<Radian>(180.0_deg) << "\n";
    std::cout << "pi rad in degrees: " << Angle::to<Degree>(3.14159_rad) << "\n";
    std::cout << "pi in rad: " << Angle::to<Radian>(pi) << "\n";
    std::cout << "pi + 90 deg in degrees: " << Angle::to<Degree>(pi + 90.0_deg) << "\n";

    std::cout << "Angle cos(M_PI * 0.5) = 0: " << std::cos(ccunits::constants::pi * 0.5) << "\n";
    std::cout << "Angle cos(90 deg): = 0: " << ccunits::math::cos(90.0_deg) << "\n";
    std::cout << "Angle acos(0) = 90 deg: " << Angle::to<Degree>(ccunits::math::acos(0)) << "\n";

    std::cout << "Angle atan2(1, 1) = 45 deg: " << Angle::to<Degree>(ccunits::math::atan2(1, 1)) << "\n";
    std::cout << "Angle atan2(1, 0) = 90 deg: " << Angle::to<Degree>(ccunits::math::atan2(1, 0)) << "\n";

    constexpr Temperature temp = Temperature::from<Kelvin>(5.0);
    std::cout << "Temp: " << Temperature::to<Celsius>(temp) << "\n";
    std::cout << "Temp 5K in Celsius: " << Temperature::to<Celsius>(5.0_K) << "\n";
    std::cout << "Temp 5K in Kelvin: " << Temperature::to<Kelvin>(5.0_K) << "\n";
    std::cout << "Temp 20C in Celsius: " << Temperature::to<Celsius>(20.0_C) << "\n";
    std::cout << "Temp 20C in Kelvin: " << Temperature::to<Kelvin>(20.0_C) << "\n";

    std::cout << "Acceleration 5mps/1s = 5m/s^2: " << Acceleration::to<MetersPerSecondSquared>(5.0_mps / 1.0_s) << "\n";
    std::cout << "Momentum 10kg * 5mps = 50Ns: " << Momentum::to<NewtonSecond>(10.0_kg * 5_mps) << "\n";

    std::cout << "Force 20kg * 5mps/s = 100N: " << Force::to<Newton>(20.0_kg * 5.0_mps / 1.0_s) << "\n";

    std::cout << "Energy 10N * 5m = 50J: " << Energy::to<Joule>(10.0_N * 5.0_m) << "\n";
    std::cout << "Energy 10W * 1s = 10Ws: " << Energy::to<Watt>(10.0_W * 1.0_s) << "\n";

    std::cout << "Power 10Ws / 5s = 2W: " << Power::to<Watt>(10.0_W * 1.0_s / 5.0_s) << "\n";
    std::cout << "Power 10kWh / 5h = 2kW: " << Power::to<Kilowatt>(10.0_W * 1000.0f * 1.0_hr / 5.0_hr) << "\n";

    std::cout << "Volume 5m^3 / 2m = 2.5m^2: " << Area::to<SquareMeter>(5_m3 / 2_m) << "\n";
    std::cout << "Volume 5m^3 / 2m^2 = 2.5m: " << Length::to<Meter>(5_m3 / 2_m2) << "\n";

    std::cout << "Pressure 1bar in mbar: " << Pressure::to<Millibar>(1_bar) << "\n";
    std::cout << "Pressure 1bar in hectopascal: " << Pressure::to<Hectopascal>(1_bar) << "\n";
    std::cout << "Pressure 5N / 10cm^2 = 5000 Pa: " << Pressure::to<Pascal>(5_N / 10_cm2) << "\n";

    std::cout << "1km < 2000m = true: " << (1_km < 2000_m ? "true" : "false") << "\n";

    // Double comparison, not stable
    std::cout << "36m/h == 10m/s = true: " << (36_kmph == 10_mps ? "true" : "false") << "\n";
    std::cout << "37m/h == 10m/s = false: " << (37_kmph == 10_mps ? "true" : "false") << "\n";
}
