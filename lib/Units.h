#include <chrono>
#include <cmath>
#include <iostream>
#include <map>
#include <ratio>
#include <string>

using namespace std::chrono_literals;

namespace ccunits {
namespace constants {
constexpr double pi = 3.14159265359;
}

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

#define DEFINE_SCALAR_MULTIPLICATION(UnitType)                           \
    constexpr UnitType operator*(const UnitType &lhs, const float rhs) { \
        UnitType u{};                                                    \
        u._value = lhs._value * rhs;                                     \
        return u;                                                        \
    }                                                                    \
                                                                         \
    constexpr UnitType operator*(const float lhs, const UnitType &rhs) { \
        UnitType u{};                                                    \
        u._value = rhs._value * lhs;                                     \
        return u;                                                        \
    }

#define DEFINE_ADDITION_OPERATOR(UnitType)                                   \
    constexpr UnitType operator+(const UnitType &lhs, const UnitType &rhs) { \
        UnitType r{};                                                        \
        r._value = lhs._value + rhs._value;                                  \
        return r;                                                            \
    }

#define DEFINE_SUBTRACTION_OPERATOR(UnitType)                                \
    constexpr UnitType operator-(const UnitType &lhs, const UnitType &rhs) { \
        UnitType r{};                                                        \
        r._value = lhs._value - rhs._value;                                  \
        return r;                                                            \
    }

#define DEFINE_NEGATIVE_OPERATOR(UnitType)            \
    constexpr UnitType operator-(const UnitType &v) { \
        UnitType r{};                                 \
        r._value = -v._value;                         \
        return r;                                     \
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

#define DEFINE_SELF_DIVISION(UnitType)                                     \
    constexpr double operator/(const UnitType &lhs, const UnitType &rhs) { \
        return lhs._value / rhs._value;                                    \
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
    namespace math {                                 \
    UnitType::Rep FunctionName(const Angle &a) {     \
        return std::FunctionName(a._value);          \
    }                                                \
    }

#define DEFINE_INVERSE_MATH_FUNCTION(UnitType, FunctionName) \
    namespace math {                                         \
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
    DEFINE_SUBTRACTION_OPERATOR(Name)  \
    DEFINE_NEGATIVE_OPERATOR(Name)     \
    DEFINE_COMPARISONS(Name)           \
    DEFINE_SELF_DIVISION(Name)

#define DEFINE_LITERAL(UnitType, Unit, Postfix)                                            \
    namespace literals {                                                                   \
    constexpr UnitType operator""_##Postfix(const long double value) noexcept {            \
        return UnitType::from<Unit>(value);                                                \
    }                                                                                      \
                                                                                           \
    constexpr UnitType operator""_##Postfix(const unsigned long long int value) noexcept { \
        return UnitType::from<Unit>(value);                                                \
    }                                                                                      \
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
DEFINE_CONVERSION_REAL_RATIO(Angle, Degree, deg, ccunits::constants::pi, 180)

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

namespace math {
Angle atan2(const double y, const double x) {
    Angle a;
    a._value = std::atan2(y, x);
    return a;
}
} // namespace math
} // namespace ccunits
