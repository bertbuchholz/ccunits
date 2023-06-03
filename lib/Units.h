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

template<typename QuantityType_>
class Quantity {
public:
    using Rep = double;
    using QuantityType = QuantityType_;

    Rep _value;

    template<
        typename T,
        std::enable_if_t<
            std::is_integral<decltype(T::Conversion::den)>::value
                && std::is_same_v<QuantityType, typename T::QuantityType>,
            bool> = true>
    constexpr static QuantityType from(const Rep value) {
        QuantityType unit{};
        unit._value = value * static_cast<Rep>(T::Conversion::num) / static_cast<Rep>(T::Conversion::den);
        return unit;
    }

    template<
        typename T,
        std::enable_if_t<
            std::is_integral<decltype(T::Conversion::factor)>::value
                && std::is_same_v<QuantityType, typename T::QuantityType>,
            bool> = true>
    constexpr static QuantityType from(const Rep value) {
        QuantityType unit{};
        unit._value = value * (static_cast<Rep>(T::Conversion::factor) / static_cast<Rep>(T::Conversion::scale))
                      + (static_cast<Rep>(T::Conversion::offset) / static_cast<Rep>(T::Conversion::scale));
        return unit;
    }

    template<
        typename T,
        std::enable_if_t<
            std::is_integral<decltype(T::Conversion::den)>::value
                && std::is_same_v<QuantityType, typename T::QuantityType>,
            bool> = true>
    constexpr static Rep to(const QuantityType &unit) {
        return unit._value * static_cast<Rep>(T::Conversion::den) / static_cast<Rep>(T::Conversion::num);
    }

    template<
        typename T,
        std::enable_if_t<
            std::is_integral<decltype(T::Conversion::factor)>::value
                && std::is_same_v<QuantityType, typename T::QuantityType>,
            bool> = true>
    constexpr static Rep to(const QuantityType &unit) {
        return unit._value / (static_cast<Rep>(T::Conversion::factor) / static_cast<Rep>(T::Conversion::scale))
               - (static_cast<Rep>(T::Conversion::offset) / static_cast<Rep>(T::Conversion::scale));
    }

    // TODO: Reenable if needed/wanted
    // template<
    //     typename T,
    //     std::enable_if_t<
    //         std::is_integral<decltype(T::Conversion::factor)>::value
    //             && std::is_same_v<QuantityType, typename T::QuantityType>,
    //         bool> = true>
    // constexpr Rep to() {
    //     return to<T>(*this);
    // }
};

#define DEFINE_SCALAR_MULTIPLICATION(QuantityType)                                      \
    inline constexpr QuantityType operator*(const QuantityType &lhs, const float rhs) { \
        QuantityType u{};                                                               \
        u._value = lhs._value * rhs;                                                    \
        return u;                                                                       \
    }                                                                                   \
                                                                                        \
    inline constexpr QuantityType operator*(const float lhs, const QuantityType &rhs) { \
        QuantityType u{};                                                               \
        u._value = rhs._value * lhs;                                                    \
        return u;                                                                       \
    }

#define DEFINE_ADDITION_OPERATOR(QuantityType)                                                  \
    inline constexpr QuantityType operator+(const QuantityType &lhs, const QuantityType &rhs) { \
        QuantityType r{};                                                                       \
        r._value = lhs._value + rhs._value;                                                     \
        return r;                                                                               \
    }

#define DEFINE_SUBTRACTION_OPERATOR(QuantityType)                                               \
    inline constexpr QuantityType operator-(const QuantityType &lhs, const QuantityType &rhs) { \
        QuantityType r{};                                                                       \
        r._value = lhs._value - rhs._value;                                                     \
        return r;                                                                               \
    }

#define DEFINE_NEGATIVE_OPERATOR(QuantityType)                       \
    inline constexpr QuantityType operator-(const QuantityType &v) { \
        QuantityType r{};                                            \
        r._value = -v._value;                                        \
        return r;                                                    \
    }

#define DEFINE_MULTIPLICATION_OPERATOR(QuantityTypeResult, QuantityType1, QuantityType2)                \
    inline constexpr QuantityTypeResult operator*(const QuantityType1 &lhs, const QuantityType2 &rhs) { \
        QuantityTypeResult r{};                                                                         \
        r._value = lhs._value * rhs._value;                                                             \
        return r;                                                                                       \
    }                                                                                                   \
                                                                                                        \
    inline constexpr QuantityTypeResult operator*(const QuantityType2 &lhs, const QuantityType1 &rhs) { \
        QuantityTypeResult r{};                                                                         \
        r._value = lhs._value * rhs._value;                                                             \
        return r;                                                                                       \
    }

#define DEFINE_MULTIPLICATION_SQUARE_OPERATOR(QuantityTypeResult, QuantityType)                       \
    inline constexpr QuantityTypeResult operator*(const QuantityType &lhs, const QuantityType &rhs) { \
        QuantityTypeResult r{};                                                                       \
        r._value = lhs._value * rhs._value;                                                           \
        return r;                                                                                     \
    }

#define DEFINE_DIVISION_OPERATOR(QuantityTypeResult, QuantityType1, QuantityType2)                      \
    inline constexpr QuantityTypeResult operator/(const QuantityType1 &lhs, const QuantityType2 &rhs) { \
        QuantityTypeResult r{};                                                                         \
        r._value = lhs._value / rhs._value;                                                             \
        return r;                                                                                       \
    }

#define DEFINE_SELF_DIVISION(QuantityType)                                                \
    inline constexpr double operator/(const QuantityType &lhs, const QuantityType &rhs) { \
        return lhs._value / rhs._value;                                                   \
    }

#define DEFINE_COMPARISONS(QuantityType)                                       \
    inline bool operator==(QuantityType const &lhs, QuantityType const &rhs) { \
        return lhs._value == rhs._value;                                       \
    }                                                                          \
                                                                               \
    inline bool operator!=(QuantityType const &lhs, QuantityType const &rhs) { \
        return lhs._value != rhs._value;                                       \
    }                                                                          \
    inline bool operator<(QuantityType const &lhs, QuantityType const &rhs) {  \
        return lhs._value < rhs._value;                                        \
    }                                                                          \
                                                                               \
    inline bool operator>(QuantityType const &lhs, QuantityType const &rhs) {  \
        return lhs._value > rhs._value;                                        \
    }                                                                          \
    inline bool operator<=(QuantityType const &lhs, QuantityType const &rhs) { \
        return lhs._value <= rhs._value;                                       \
    }                                                                          \
                                                                               \
    inline bool operator>=(QuantityType const &lhs, QuantityType const &rhs) { \
        return lhs._value >= rhs._value;                                       \
    }

#define DEFINE_MATH_FUNCTION(QuantityType, FunctionName)    \
    namespace math {                                        \
    inline QuantityType::Rep FunctionName(const Angle &a) { \
        return std::FunctionName(a._value);                 \
    }                                                       \
    }

#define DEFINE_INVERSE_MATH_FUNCTION(QuantityType, FunctionName)      \
    namespace math {                                                  \
    inline QuantityType FunctionName(const QuantityType::Rep value) { \
        QuantityType a;                                               \
        a._value = std::FunctionName(value);                          \
        return a;                                                     \
    }                                                                 \
    }

#define REGISTER_QUANTITY(Name)            \
    class Name : public Quantity<Name> {}; \
    DEFINE_SCALAR_MULTIPLICATION(Name)     \
    DEFINE_ADDITION_OPERATOR(Name)         \
    DEFINE_SUBTRACTION_OPERATOR(Name)      \
    DEFINE_NEGATIVE_OPERATOR(Name)         \
    DEFINE_COMPARISONS(Name)               \
    DEFINE_SELF_DIVISION(Name)

#define DEFINE_LITERAL(QuantityType, Unit, Postfix)                                                   \
    namespace literals {                                                                              \
    inline constexpr QuantityType operator""_##Postfix(const long double value) noexcept {            \
        return QuantityType::from<Unit>(static_cast<QuantityType::Rep>(value));                       \
    }                                                                                                 \
                                                                                                      \
    inline constexpr QuantityType operator""_##Postfix(const unsigned long long int value) noexcept { \
        return QuantityType::from<Unit>(static_cast<QuantityType::Rep>(value));                       \
    }                                                                                                 \
    }

#define DEFINE_RELATION(QuantityTypeResult, QuantityType1, QuantityType2)            \
    DEFINE_MULTIPLICATION_OPERATOR(QuantityTypeResult, QuantityType1, QuantityType2) \
    DEFINE_DIVISION_OPERATOR(QuantityType1, QuantityTypeResult, QuantityType2)       \
    DEFINE_DIVISION_OPERATOR(QuantityType2, QuantityTypeResult, QuantityType1)

#define DEFINE_UNIT(QuantityType_, UnitName, Literal, Numerator, Denominator)                                \
    struct UnitName {                                                                                        \
        using Conversion = std::ratio<static_cast<long int>(Numerator), static_cast<long int>(Denominator)>; \
        using QuantityType = QuantityType_;                                                                  \
    };                                                                                                       \
    DEFINE_LITERAL(QuantityType_, UnitName, Literal)

#define DEFINE_UNIT_WITH_RATIO(QuantityType_, UnitName, Literal, Ratio) \
    struct UnitName {                                                   \
        using Conversion = Ratio;                                       \
        using QuantityType = QuantityType_;                             \
    };                                                                  \
    DEFINE_LITERAL(QuantityType_, UnitName, Literal)

#define DEFINE_UNIT_REAL_RATIO(QuantityType_, UnitName, Literal, Numerator, Denominator)                    \
    struct UnitName {                                                                                       \
        using Conversion =                                                                                  \
            std::ratio<static_cast<uint64_t>(Numerator * 1e10), static_cast<uint64_t>(Denominator * 1e10)>; \
        using QuantityType = QuantityType_;                                                                 \
    };                                                                                                      \
    DEFINE_LITERAL(QuantityType_, UnitName, Literal)

#define DEFINE_UNIT_OFFSET(QuantityType_, UnitName, Literal, Factor, Offset) \
    struct UnitName {                                                        \
        using Conversion = Conversion<                                       \
            static_cast<uint64_t>(Factor * 1e10L),                           \
            static_cast<uint64_t>(Offset * 1e10L),                           \
            static_cast<uint64_t>(1e10L)>;                                   \
        using QuantityType = QuantityType_;                                  \
    };                                                                       \
    DEFINE_LITERAL(QuantityType_, UnitName, Literal)

REGISTER_QUANTITY(Frequency) // TODO: Add literals, relations and use cases
REGISTER_QUANTITY(Length)
REGISTER_QUANTITY(Area)
REGISTER_QUANTITY(Volume)
REGISTER_QUANTITY(Speed)
REGISTER_QUANTITY(Acceleration)
REGISTER_QUANTITY(Mass)
REGISTER_QUANTITY(Angle)
REGISTER_QUANTITY(TemperatureDifference)
REGISTER_QUANTITY(Momentum)
REGISTER_QUANTITY(Force)
REGISTER_QUANTITY(Power)
REGISTER_QUANTITY(Energy)
REGISTER_QUANTITY(Pressure)

// Temperature is actually a TemperaturePoint and its meaning is equivalent
// to that of time point and duration
class Temperature : public Quantity<Temperature> {};
DEFINE_SCALAR_MULTIPLICATION(Temperature)
DEFINE_SELF_DIVISION(Temperature)
DEFINE_COMPARISONS(Temperature)

class Duration : public Quantity<Duration> {
public:
    Duration() = default;

    template<typename T>
    constexpr Duration(const T d)
        : Quantity<Duration>() {
        const auto durationDouble = std::chrono::duration_cast<std::chrono::duration<double>>(d);
        _value = durationDouble.count();
    }
};
DEFINE_SCALAR_MULTIPLICATION(Duration)
DEFINE_ADDITION_OPERATOR(Duration)
DEFINE_SUBTRACTION_OPERATOR(Duration)
DEFINE_NEGATIVE_OPERATOR(Duration)
DEFINE_COMPARISONS(Duration)
DEFINE_SELF_DIVISION(Duration)

// This definition is not possible since DEFINE_RELATION defines the multiplication
// operator twice for lhs * rhs and rhs * lhs which leads to a double defintion if
// both quantities are the same
// DEFINE_RELATION(Area, Length, Length)
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

inline constexpr TemperatureDifference operator-(const Temperature &lhs, const Temperature &rhs) {
    TemperatureDifference r{};
    r._value = lhs._value - rhs._value;
    return r;
}

inline constexpr Temperature operator+(const Temperature &lhs, const TemperatureDifference &rhs) {
    Temperature r{};
    r._value = lhs._value + rhs._value;
    return r;
}

inline constexpr Temperature operator-(const Temperature &lhs, const TemperatureDifference &rhs) {
    Temperature r{};
    r._value = lhs._value - rhs._value;
    return r;
}

DEFINE_UNIT_WITH_RATIO(Duration, Nanosecond, ns, std::nano)
DEFINE_UNIT_WITH_RATIO(Duration, Microsecond, us, std::micro)
DEFINE_UNIT_WITH_RATIO(Duration, Millisecond, ms, std::milli)
DEFINE_UNIT_WITH_RATIO(Duration, Second, s, std::ratio<1>)
DEFINE_UNIT(Duration, Minute, min, 60, 1)
DEFINE_UNIT(Duration, Hour, hr, 60 * 60, 1)
DEFINE_UNIT(Duration, Day, day, 60 * 60 * 24, 1)

DEFINE_UNIT_WITH_RATIO(Length, Nanometer, nm, std::nano)
DEFINE_UNIT_WITH_RATIO(Length, Micrometer, um, std::micro)
DEFINE_UNIT_WITH_RATIO(Length, Millimeter, mm, std::milli)
DEFINE_UNIT_WITH_RATIO(Length, Centimeter, cm, std::centi)
DEFINE_UNIT_WITH_RATIO(Length, Meter, m, std::ratio<1>)
DEFINE_UNIT_WITH_RATIO(Length, Kilometer, km, std::kilo)

// This is a workaround for commas in template arguments which are parsed as multiple macro arguments.
// Putting the template argument in () to make it appear as a single argument and using a clever template construct
// to derive the template argument by itself fails with MSVC.
#define COMMA ,
DEFINE_UNIT_WITH_RATIO(
    Area,
    SquareMillimeter,
    mm2,
    std::ratio_multiply<Millimeter::Conversion COMMA Millimeter::Conversion>)
DEFINE_UNIT_WITH_RATIO(
    Area,
    SquareCentimeter,
    cm2,
    std::ratio_multiply<Centimeter::Conversion COMMA Centimeter::Conversion>)
DEFINE_UNIT_WITH_RATIO(Area, SquareMeter, m2, std::ratio<1>)
DEFINE_UNIT_WITH_RATIO(
    Area,
    SquareKilometer,
    km2,
    std::ratio_multiply<Kilometer::Conversion COMMA Kilometer::Conversion>)

DEFINE_UNIT_WITH_RATIO(
    Volume,
    CubicMillimeter,
    nm3,
    std::ratio_multiply<SquareMillimeter::Conversion COMMA Millimeter::Conversion>)
DEFINE_UNIT_WITH_RATIO(Volume, CubicMeter, m3, std::ratio<1>)

DEFINE_UNIT_WITH_RATIO(Speed, MetersPerSecond, mps, std::ratio<1>)
DEFINE_UNIT_WITH_RATIO(
    Speed,
    KilometersPerHour,
    kmph,
    std::ratio_divide<Kilometer::Conversion COMMA std::chrono::hours::period>)

DEFINE_UNIT(Angle, Radian, rad, 1, 1)
DEFINE_UNIT_REAL_RATIO(Angle, Degree, deg, ccunits::constants::pi, 180)

DEFINE_UNIT(Temperature, Kelvin, K, 1, 1)
// Don't provide a literal C because literals are unsigned only. If a user specifies -5_C, first 5 will be converted to
// Kelvin -> 278K and then the minus applied, resulting in -278K (instead of the expected 268K).
// DEFINE_UNIT_OFFSET(Temperature, Celsius, C, 1, 273.15)
struct Celsius {
    using Conversion =
        Conversion<static_cast<uint64_t>(1e10L), static_cast<uint64_t>(273.15 * 1e10L), static_cast<uint64_t>(1e10L)>;
    using QuantityType = Temperature;
};

DEFINE_UNIT(TemperatureDifference, dKelvin, dK, 1, 1)
DEFINE_UNIT(TemperatureDifference, dCelsius, dC, 1, 1)

DEFINE_UNIT(Pressure, Pascal, Pa, 1, 1)
DEFINE_UNIT(Pressure, Hectopascal, hPa, 100, 1)
DEFINE_UNIT(Pressure, Bar, bar, 100000, 1)
DEFINE_UNIT(Pressure, Millibar, mbar, 100, 1)

// Mass
DEFINE_UNIT_WITH_RATIO(Mass, Gram, gr, std::milli)
DEFINE_UNIT_WITH_RATIO(Mass, Kilogram, kg, std::ratio<1>)

// Power
DEFINE_UNIT_WITH_RATIO(Power, Microwatt, uW, std::micro)
DEFINE_UNIT_WITH_RATIO(Power, Milliwatt, mW, std::milli)
DEFINE_UNIT_WITH_RATIO(Power, Watt, W, std::ratio<1>)
DEFINE_UNIT_WITH_RATIO(Power, Kilowatt, kW, std::kilo)
DEFINE_UNIT_WITH_RATIO(Power, Megawatt, MW, std::mega)
DEFINE_UNIT_WITH_RATIO(Power, Gigawatt, GW, std::giga)

// Energy
DEFINE_UNIT_WITH_RATIO(Energy, Joule, J, std::ratio<1>)
DEFINE_UNIT_WITH_RATIO(Energy, WattSecond, Ws, std::ratio_multiply<Watt::Conversion COMMA Second::Conversion>)
DEFINE_UNIT_WITH_RATIO(Energy, WattHour, Wh, std::ratio_multiply<Watt::Conversion COMMA Hour::Conversion>)
DEFINE_UNIT_WITH_RATIO(Energy, KilowattHour, kWh, std::ratio_multiply<std::kilo COMMA WattHour::Conversion>)

// Force
DEFINE_UNIT_WITH_RATIO(Force, Newton, N, std::ratio<1>)

// Acceleration
DEFINE_UNIT_WITH_RATIO(Acceleration, MetersPerSecondSquared, mps2, std::ratio<1>)

// Momentum
DEFINE_UNIT_WITH_RATIO(Momentum, NewtonSecond, Ns, std::ratio<1>)
DEFINE_UNIT_WITH_RATIO(Momentum, KilogramMeterPerSecond, kgmps, std::ratio<1>)

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
inline Angle atan2(const double y, const double x) {
    Angle a;
    a._value = std::atan2(y, x);
    return a;
}
} // namespace math
} // namespace ccunits
