#define _USE_MATH_DEFINES

#include <chrono>
#include <cmath>
#include <iostream>
#include <map>
#include <ratio>
#include <string>

using namespace std::chrono_literals;

template<long long int factor_, long long int offset_, long long int scale_>
struct Conversion {
    constexpr static long long int factor = factor_;
    constexpr static long long int offset = offset_;
    constexpr static long long int scale = scale_;
};

template<typename UnitType>
class Unit {
public:
    using Rep = double;

    Rep _value;

    template<typename T, std::enable_if_t<std::is_integral<decltype(T::den)>::value, bool> = true>
    constexpr static UnitType from(const Rep value) {
        UnitType unit{};
        unit._value = value * static_cast<Rep>(T::num) / static_cast<Rep>(T::den);
        return unit;
    }

    template<typename T, std::enable_if_t<std::is_integral<decltype(T::factor)>::value, bool> = true>
    constexpr static UnitType from(const Rep value) {
        UnitType unit{};
        unit._value = value * (static_cast<Rep>(T::factor) / static_cast<Rep>(T::scale))
                      + (static_cast<Rep>(T::offset) / static_cast<Rep>(T::scale));
        return unit;
    }

    template<typename T, std::enable_if_t<std::is_integral<decltype(T::den)>::value, bool> = true>
    constexpr static Rep to(const UnitType &unit) {
        return unit._value * static_cast<Rep>(T::den) / static_cast<Rep>(T::num);
    }

    template<typename T, std::enable_if_t<std::is_integral<decltype(T::factor)>::value, bool> = true>
    constexpr static Rep to(const UnitType &unit) {
        return unit._value / (static_cast<Rep>(T::factor) / static_cast<Rep>(T::scale))
               - (static_cast<Rep>(T::offset) / static_cast<Rep>(T::scale));
    }
};

#define DEFINE_SCALAR_MULTIPLICATION(UnitType)                 \
    UnitType operator*(const UnitType &lhs, const float rhs) { \
        UnitType u;                                            \
        u._value = lhs._value * rhs;                           \
        return u;                                              \
    }                                                          \
                                                               \
    UnitType operator*(const float lhs, const UnitType &rhs) { \
        UnitType u;                                            \
        u._value = rhs._value * lhs;                           \
        return u;                                              \
    }

#define DEFINE_ADDITION_OPERATOR(UnitType)                         \
    UnitType operator+(const UnitType &lhs, const UnitType &rhs) { \
        UnitType r;                                                \
        r._value = lhs._value + rhs._value;                        \
        return r;                                                  \
    }

#define DEFINE_SUBTRACTION_OPERATOR(UnitType)                      \
    UnitType operator-(const UnitType &lhs, const UnitType &rhs) { \
        UnitType r;                                                \
        r._value = lhs._value - rhs._value;                        \
        return r;                                                  \
    }

#define DEFINE_MULTIPLICATION_OPERATOR(UnitTypeResult, UnitType1, UnitType2)         \
    constexpr UnitTypeResult operator*(const UnitType1 &lhs, const UnitType2 &rhs) { \
        UnitTypeResult r{};                                                          \
        r._value = lhs._value * rhs._value;                                          \
        return r;                                                                    \
    }                                                                                \
                                                                                     \
    constexpr UnitTypeResult operator*(const UnitType2 &lhs, const UnitType1 &rhs) { \
        UnitTypeResult r{};                                                          \
        r._value = lhs._value * rhs._value;                                          \
        return r;                                                                    \
    }

#define DEFINE_MULTIPLICATION_SQUARE_OPERATOR(UnitTypeResult, UnitType)            \
    constexpr UnitTypeResult operator*(const UnitType &lhs, const UnitType &rhs) { \
        UnitTypeResult r{};                                                        \
        r._value = lhs._value * rhs._value;                                        \
        return r;                                                                  \
    }

#define DEFINE_DIVISION_OPERATOR(UnitTypeResult, UnitType1, UnitType2)               \
    constexpr UnitTypeResult operator/(const UnitType1 &lhs, const UnitType2 &rhs) { \
        UnitTypeResult r{};                                                          \
        r._value = lhs._value / rhs._value;                                          \
        return r;                                                                    \
    }

#define DEFINE_COMPARISONS(UnitType)                            \
    bool operator==(UnitType const &lhs, UnitType const &rhs) { \
        return lhs._value == rhs._value;                        \
    }                                                           \
                                                                \
    bool operator!=(UnitType const &lhs, UnitType const &rhs) { \
        return lhs._value != rhs._value;                        \
    }                                                           \
    bool operator<(UnitType const &lhs, UnitType const &rhs) {  \
        return lhs._value < rhs._value;                         \
    }                                                           \
                                                                \
    bool operator>(UnitType const &lhs, UnitType const &rhs) {  \
        return lhs._value > rhs._value;                         \
    }                                                           \
    bool operator<=(UnitType const &lhs, UnitType const &rhs) { \
        return lhs._value <= rhs._value;                        \
    }                                                           \
                                                                \
    bool operator>=(UnitType const &lhs, UnitType const &rhs) { \
        return lhs._value >= rhs._value;                        \
    }

#define DEFINE_MATH_FUNCTION(UnitType, FunctionName) \
    namespace units {                                \
    UnitType::Rep FunctionName(const Angle &a) {     \
        return std::FunctionName(a._value);          \
    }                                                \
    }

#define DEFINE_INVERSE_MATH_FUNCTION(UnitType, FunctionName) \
    namespace units {                                        \
    UnitType FunctionName(const UnitType::Rep value) {       \
        UnitType a;                                          \
        a._value = std::FunctionName(value);                 \
        return a;                                            \
    }                                                        \
    }

#define REGISTER_UNIT(Name)            \
    class Name : public Unit<Name> {}; \
    DEFINE_SCALAR_MULTIPLICATION(Name) \
    DEFINE_ADDITION_OPERATOR(Name)     \
    DEFINE_COMPARISONS(Name)

#define DEFINE_LITERAL(UnitType, Unit, Postfix)                                            \
    constexpr UnitType operator""_##Postfix(const long double value) noexcept {            \
        return UnitType::from<Unit>(value);                                                \
    }                                                                                      \
                                                                                           \
    constexpr UnitType operator""_##Postfix(const unsigned long long int value) noexcept { \
        return UnitType::from<Unit>(value);                                                \
    }

#define DEFINE_RELATION(UnitTypeResult, UnitType1, UnitType2)            \
    DEFINE_MULTIPLICATION_OPERATOR(UnitTypeResult, UnitType1, UnitType2) \
    DEFINE_DIVISION_OPERATOR(UnitType1, UnitTypeResult, UnitType2)       \
    DEFINE_DIVISION_OPERATOR(UnitType2, UnitTypeResult, UnitType1)

#define DEFINE_CONVERSION(UnitType, ConversionName, Literal, Numerator, Denominator)                         \
    using ConversionName = std::ratio<static_cast<long int>(Numerator), static_cast<long int>(Denominator)>; \
    DEFINE_LITERAL(UnitType, ConversionName, Literal)

// Needs some messing with templates for types that contain a comma, like ratio<1, 2>,
// see https://stackoverflow.com/questions/13842468/comma-in-c-c-macro
template<typename T>
struct argument_type;
template<typename T, typename U>
struct argument_type<T(U)> {
    using type = U;
};

#define DEFINE_CONVERSION_WITH_RATIO(UnitType, ConversionName, Literal, Ratio) \
    using ConversionName = Ratio;                                              \
    DEFINE_LITERAL(UnitType, ConversionName, Literal)

#define DEFINE_CONVERSION_REAL_RATIO(UnitType, ConversionName, Literal, Numerator, Denominator)         \
    using ConversionName =                                                                              \
        std::ratio<static_cast<uint64_t>(Numerator * 1e10), static_cast<uint64_t>(Denominator * 1e10)>; \
    DEFINE_LITERAL(UnitType, ConversionName, Literal)

#define DEFINE_CONVERSION_OFFSET(UnitType, ConversionName, Literal, Factor, Offset) \
    using ConversionName = Conversion<                                              \
        static_cast<uint64_t>(Factor * 1e10L),                                      \
        static_cast<uint64_t>(Offset * 1e10L),                                      \
        static_cast<uint64_t>(1e10L)>;                                              \
    DEFINE_LITERAL(UnitType, ConversionName, Literal)

REGISTER_UNIT(Frequency) // TODO: Add literals, relations and use cases
REGISTER_UNIT(Length)
REGISTER_UNIT(Area)
REGISTER_UNIT(Volume)
REGISTER_UNIT(Speed)
REGISTER_UNIT(Acceleration)
REGISTER_UNIT(Mass)
REGISTER_UNIT(Angle)
REGISTER_UNIT(Temperature)
REGISTER_UNIT(Momentum)
REGISTER_UNIT(Force)
REGISTER_UNIT(Power)
REGISTER_UNIT(Energy)
REGISTER_UNIT(Pressure)

class Duration : public Unit<Duration> {
public:
    Duration() = default;

    template<typename T>
    constexpr Duration(const T d)
        : Unit<Duration>() {
        const auto durationDouble = std::chrono::duration_cast<std::chrono::duration<double>>(d);
        _value = durationDouble.count();
    }
};

DEFINE_SCALAR_MULTIPLICATION(Duration)
DEFINE_ADDITION_OPERATOR(Duration)

DEFINE_MULTIPLICATION_SQUARE_OPERATOR(Area, Length)

// Volume = Area * Length and derived divisions etc.
DEFINE_RELATION(Volume, Area, Length)
DEFINE_RELATION(Speed, Acceleration, Duration)
DEFINE_RELATION(Length, Speed, Duration)
DEFINE_RELATION(Energy, Power, Duration)
DEFINE_RELATION(Energy, Force, Length)
DEFINE_RELATION(Force, Mass, Acceleration)
DEFINE_RELATION(Force, Pressure, Area)
DEFINE_RELATION(Momentum, Mass, Speed)
DEFINE_RELATION(Momentum, Force, Duration)

DEFINE_CONVERSION_WITH_RATIO(Duration, Nanosecond, ns, std::nano)
DEFINE_CONVERSION_WITH_RATIO(Duration, Microsecond, us, std::micro)
DEFINE_CONVERSION_WITH_RATIO(Duration, Millisecond, ms, std::milli)
DEFINE_CONVERSION_WITH_RATIO(Duration, Second, s, std::ratio<1>)
DEFINE_CONVERSION(Duration, Minute, min, 60, 1)
DEFINE_CONVERSION(Duration, Hour, hr, 60 * 60, 1)
DEFINE_CONVERSION(Duration, Day, day, 60 * 60 * 24, 1)

DEFINE_CONVERSION_WITH_RATIO(Length, Nanometer, nm, std::nano)
DEFINE_CONVERSION_WITH_RATIO(Length, Micrometer, um, std::micro)
DEFINE_CONVERSION_WITH_RATIO(Length, Millimeter, mm, std::milli)
DEFINE_CONVERSION_WITH_RATIO(Length, Centimeter, cm, std::centi)
DEFINE_CONVERSION_WITH_RATIO(Length, Meter, m, std::ratio<1>)
DEFINE_CONVERSION_WITH_RATIO(Length, Kilometer, km, std::kilo)

// This is a workaround for commas in template arguments which are parsed as multiple macro arguments.
// Putting the template argument in () to make it appear as a single argument and using a clever template construct
// to derive the template argument by itself fails with MSVC.
#define COMMA ,
DEFINE_CONVERSION_WITH_RATIO(Area, SquareMillimeter, mm2, std::ratio_multiply<Millimeter COMMA Millimeter>)
DEFINE_CONVERSION_WITH_RATIO(Area, SquareCentimeter, cm2, std::ratio_multiply<Centimeter COMMA Centimeter>)
DEFINE_CONVERSION_WITH_RATIO(Area, SquareMeter, m2, std::ratio<1>)
DEFINE_CONVERSION_WITH_RATIO(Area, SquareKilometer, km2, std::ratio_multiply<Kilometer COMMA Kilometer>)

DEFINE_CONVERSION_WITH_RATIO(Volume, CubicMillimeter, nm3, std::ratio_multiply<SquareMillimeter COMMA Millimeter>)
DEFINE_CONVERSION_WITH_RATIO(Volume, CubicMeter, m3, std::ratio<1>)

DEFINE_CONVERSION_WITH_RATIO(Speed, MetersPerSecond, mps, std::ratio<1>)
DEFINE_CONVERSION_WITH_RATIO(
    Speed,
    KilometersPerHour,
    kmph,
    std::ratio_divide<Kilometer COMMA std::chrono::hours::period>)

DEFINE_CONVERSION(Angle, Radian, rad, 1, 1)
DEFINE_CONVERSION_REAL_RATIO(Angle, Degree, deg, M_PI, 180)

DEFINE_CONVERSION(Temperature, Kelvin, K, 1, 1)
DEFINE_CONVERSION_OFFSET(Temperature, Celsius, C, 1, 273.15)

DEFINE_CONVERSION(Pressure, Pascal, Pa, 1, 1)
DEFINE_CONVERSION(Pressure, Hectopascal, hPa, 100, 1)
DEFINE_CONVERSION(Pressure, Bar, bar, 100000, 1)
DEFINE_CONVERSION(Pressure, Millibar, mbar, 100, 1)

// Mass
DEFINE_CONVERSION_WITH_RATIO(Mass, Gram, gr, std::milli)
DEFINE_CONVERSION_WITH_RATIO(Mass, Kilogram, kg, std::ratio<1>)

// Energy
DEFINE_CONVERSION_WITH_RATIO(Energy, Joule, J, std::ratio<1>)

// Power
DEFINE_CONVERSION_WITH_RATIO(Power, Microwatt, uW, std::micro)
DEFINE_CONVERSION_WITH_RATIO(Power, Milliwatt, mW, std::milli)
DEFINE_CONVERSION_WITH_RATIO(Power, Watt, W, std::ratio<1>)
DEFINE_CONVERSION_WITH_RATIO(Power, Kilowatt, kW, std::kilo)
DEFINE_CONVERSION_WITH_RATIO(Power, Megawatt, MW, std::mega)
DEFINE_CONVERSION_WITH_RATIO(Power, Gigawatt, GW, std::giga)

// Force
DEFINE_CONVERSION_WITH_RATIO(Force, Newton, N, std::ratio<1>)

// Acceleration
DEFINE_CONVERSION_WITH_RATIO(Acceleration, MetersPerSecondSquared, mps2, std::ratio<1>)

// Momentum
DEFINE_CONVERSION_WITH_RATIO(Momentum, NewtonSecond, Ns, std::ratio<1>)
DEFINE_CONVERSION_WITH_RATIO(Momentum, KilogramMeterPerSecond, kgmps, std::ratio<1>)

DEFINE_MATH_FUNCTION(Angle, sin)
DEFINE_MATH_FUNCTION(Angle, cos)
DEFINE_MATH_FUNCTION(Angle, tan)
DEFINE_MATH_FUNCTION(Angle, sinh)
DEFINE_MATH_FUNCTION(Angle, cosh)
DEFINE_MATH_FUNCTION(Angle, tanh)
DEFINE_INVERSE_MATH_FUNCTION(Angle, asin)
DEFINE_INVERSE_MATH_FUNCTION(Angle, acos)
DEFINE_INVERSE_MATH_FUNCTION(Angle, atan)
DEFINE_INVERSE_MATH_FUNCTION(Angle, asinh)
DEFINE_INVERSE_MATH_FUNCTION(Angle, acosh)
DEFINE_INVERSE_MATH_FUNCTION(Angle, atanh)

namespace units {
Angle atan2(const double y, const double x) {
    Angle a;
    a._value = std::atan2(y, x);
    return a;
}
} // namespace units

int main() {
    // const auto someLength = 1_km;

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

    constexpr Angle pi = Angle::from<Radian>(M_PI);

    std::cout << "45 deg in radians: " << Angle::to<Radian>(45.0_deg) << "\n";
    std::cout << "90 deg in radians: " << Angle::to<Radian>(90.0_deg) << "\n";
    std::cout << "180 deg in radians: " << Angle::to<Radian>(180.0_deg) << "\n";
    std::cout << "pi rad in degrees: " << Angle::to<Degree>(3.14159_rad) << "\n";
    std::cout << "pi in rad: " << Angle::to<Radian>(pi) << "\n";
    std::cout << "pi + 90 deg in degrees: " << Angle::to<Degree>(pi + 90.0_deg) << "\n";

    std::cout << "Angle cos(M_PI * 0.5) = 0: " << std::cos(M_PI * 0.5) << "\n";
    std::cout << "Angle cos(90 deg): = 0: " << units::cos(90.0_deg) << "\n";
    std::cout << "Angle acos(0) = 90 deg: " << Angle::to<Degree>(units::acos(0)) << "\n";

    std::cout << "Angle atan2(1, 1) = 45 deg: " << Angle::to<Degree>(units::atan2(1, 1)) << "\n";
    std::cout << "Angle atan2(1, 0) = 90 deg: " << Angle::to<Degree>(units::atan2(1, 0)) << "\n";

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