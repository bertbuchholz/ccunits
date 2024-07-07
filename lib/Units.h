#ifndef CCUNITS_UNITS_H
#define CCUNITS_UNITS_H

#include <iostream>
#include <chrono>
#include <cmath>
#include <ratio>
#include <string>
#include <tuple>
#include <typeinfo>

namespace ccunits {

// Helper functions

template<double Num_, double Den_>
struct ratiod
{
    static_assert(Den_ != 0, "denominator cannot be zero");

    static constexpr double num = Num_;
    static constexpr double den = Den_;

    typedef ratiod<num, den> type;
};


template <class... T>
constexpr bool always_false = false;

template<class T>
std::string name() {
    return typeid(T).name();
}

template<class V>
void print(char const * n) {
    std::cout << n << ": " << name<V>() << "\n";
}

template<typename R, typename T, typename... Ts>
constexpr auto removeFirst(std::tuple<T, Ts...> &&) {
    if constexpr (std::is_same<R, T>::value) {
        return std::tuple<Ts...>();
    }
    else {
        return std::tuple_cat(std::tuple<T>(), removeFirst<R>(std::tuple<Ts...>()));
    }
};

template<typename V>
constexpr auto contains(std::tuple<> &&) {
    return false;
}

template<typename V, typename T, typename... Ts>
constexpr auto contains(std::tuple<T, Ts...> &&) {
    if constexpr (std::is_same_v<V, T>) {
        return true;
    }
    else {
        return contains<V>(std::tuple<Ts...>());
    }
};

template<typename... Ys>
constexpr auto removePairs(std::tuple<> &&, std::tuple<Ys...> &&) {
    return std::tuple(std::tuple<>(), std::tuple<Ys...>());
}

template<typename... Xs>
constexpr auto removePairs(std::tuple<Xs...> &&, std::tuple<> &&) {
    return std::tuple(std::tuple<Xs...>(), std::tuple<>());
}

constexpr auto removePairs(std::tuple<> &&, std::tuple<> &&) {
    return std::tuple(std::tuple<>(), std::tuple<>());
}

// TODO: We might need to sort the parameter types at the end
template<typename X, typename... Xs, typename Y, typename... Ys>
constexpr auto removePairs(std::tuple<X, Xs...> &&, std::tuple<Y, Ys...> &&) {
    if constexpr (contains<X>(std::tuple<Y, Ys...>())) {
        return removePairs(std::tuple<Xs...>(), removeFirst<X>(std::tuple<Y, Ys...>()));
    }
    else {
        constexpr auto tmp = removePairs(std::tuple<Xs...>(), std::tuple<Y, Ys...>());

        using Nom = decltype(std::tuple_cat(std::tuple<X>(), std::get<0>(tmp)));
        using Den = std::remove_cvref_t<decltype(std::get<1>(tmp))>;

        return std::tuple<Nom, Den>();
    }
};


// Quantity

template<int64_t factor_, int64_t offset_, int64_t scale_>
struct Conversion {
    constexpr static int64_t factor = factor_;
    constexpr static int64_t offset = offset_;
    constexpr static int64_t scale = scale_;
};

// Forward declare for std::chrono conversion
namespace base {
struct Duration;
}

struct BaseQuantity {};

template<typename T>
concept TupleLike = requires { std::tuple_size<T>::value; };

constexpr bool allElementsDerivedFromBaseQuantity(std::tuple<>) {
    return true;
}

template<typename X, typename... Xs>
constexpr bool allElementsDerivedFromBaseQuantity(std::tuple<X, Xs...>) {
    if constexpr (std::derived_from<X, BaseQuantity>) {
        return allElementsDerivedFromBaseQuantity(std::tuple<Xs...>());
    }
    else {
        return false;
    }
}

template<typename TupleType>
constexpr bool allElementsDerivedFromBaseQuantity() {
    return allElementsDerivedFromBaseQuantity(TupleType{});
}

template<typename TupleType>
concept BaseQuantityTuple = TupleLike<TupleType> && allElementsDerivedFromBaseQuantity<TupleType>();

template<BaseQuantityTuple Num_, BaseQuantityTuple Den_, typename AddConstraint_ = void>
struct Quantity {
    using Num = Num_;
    using Den = Den_;
    using AddConstraint = AddConstraint_;

    using Rep = double;
    Rep _value; // Always in the SI base units

    template <typename Unit>
    static constexpr Quantity from(Rep const value) requires
        std::is_arithmetic_v<decltype(Unit::Ratio::den)>
            && (std::is_same_v<Quantity, typename Unit::Quantity>
            || std::is_same_v<Quantity::AddConstraint, typename Unit::Quantity>)
    {
        Quantity q;
        q._value = value * static_cast<Rep>(Unit::Ratio::num) / static_cast<Rep>(Unit::Ratio::den);
        return q;
    }

    template <typename Unit>
    static constexpr Unit::Quantity from(Rep const value) requires(
        std::is_integral<decltype(Unit::Ratio::factor)>::value && std::is_same_v<Quantity, typename Unit::Quantity>
    ) {
        typename Unit::Quantity q;
        q._value = value * (static_cast<Rep>(Unit::Ratio::factor) / static_cast<Rep>(Unit::Ratio::scale))
                      + (static_cast<Rep>(Unit::Ratio::offset) / static_cast<Rep>(Unit::Ratio::scale));

        return q;
    }

    template <typename Unit>
    constexpr Rep to() const requires(
        std::is_arithmetic_v<decltype(Unit::Ratio::den)> && std::is_same_v<Quantity, typename Unit::Quantity>
    ) {
        return _value * static_cast<Rep>(Unit::Ratio::den) / static_cast<Rep>(Unit::Ratio::num);
    }

    template <typename Unit>
    constexpr Rep to() const requires(
            std::is_integral_v<decltype(Unit::Ratio::factor)>
            && std::is_same_v<Quantity, typename Unit::Quantity>) {
        return _value / (static_cast<Rep>(Unit::Ratio::factor) / static_cast<Rep>(Unit::Ratio::scale))
            - (static_cast<Rep>(Unit::Ratio::offset) / static_cast<Rep>(Unit::Ratio::scale));
    }

    constexpr Quantity() = default;

    // This constructor doesn't work in situations like `5_m / 1s` because the Quantity here requires
    // Num, Den etc. defined and `/ 1s` expects a certain Quantity type that it can't deduct. `5_m / Duration(1s)`
    // works because then the template parameters for Quantity are known
    template<typename... T>
    constexpr Quantity(const std::chrono::duration<T...> d)
        requires(std::is_same_v<Num, std::tuple<base::Duration>> && std::is_same_v<Den, std::tuple<>>)
    {
        const auto durationRep = std::chrono::duration_cast<std::chrono::duration<Quantity::Rep>>(d);
        _value = durationRep.count();
    }
};

template<typename Q>
concept IsQuantity = requires {
    typename Q::Den;
    typename Q::Num;
    typename Q::AddConstraint;
    typename Q::Rep;
};

template<typename Q1, typename Q2>
concept SameQuantity =
    IsQuantity<Q1>
    && IsQuantity<Q2>
    && std::is_same_v<typename Q1::Num, typename Q2::Num>
    && std::is_same_v<typename Q1::Den, typename Q2::Den>;

template<typename Q>
concept HasAddConstraint =
    IsQuantity<Q>
    && not std::is_same_v<typename Q::AddConstraint, void>;

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

#define DEFINE_UNITS(QuantityType, UnitName, UnitNameLowerCase, Postfix)           \
    struct Nano##UnitNameLowerCase  : Unit<QuantityType, std::nano> {};            \
    struct Micro##UnitNameLowerCase : Unit<QuantityType, std::micro> {};           \
    struct Milli##UnitNameLowerCase : Unit<QuantityType, std::milli> {};           \
    struct Centi##UnitNameLowerCase : Unit<QuantityType, std::centi> {};           \
    struct UnitName                 : Unit<QuantityType, std::ratio<1>> {};        \
    struct Hecto##UnitNameLowerCase : Unit<QuantityType, std::hecto> {};           \
    struct Kilo##UnitNameLowerCase  : Unit<QuantityType, std::kilo> {};            \
    struct Mega##UnitNameLowerCase  : Unit<QuantityType, std::mega> {};            \
    struct Giga##UnitNameLowerCase  : Unit<QuantityType, std::giga> {};            \
    DEFINE_LITERAL(QuantityType, Nano##UnitNameLowerCase,  n##Postfix)             \
    DEFINE_LITERAL(QuantityType, Micro##UnitNameLowerCase, u##Postfix)             \
    DEFINE_LITERAL(QuantityType, Milli##UnitNameLowerCase, m##Postfix)             \
    DEFINE_LITERAL(QuantityType, Centi##UnitNameLowerCase, c##Postfix)             \
    DEFINE_LITERAL(QuantityType, UnitName,                    Postfix)             \
    DEFINE_LITERAL(QuantityType, Hecto##UnitNameLowerCase, h##Postfix)             \
    DEFINE_LITERAL(QuantityType, Kilo##UnitNameLowerCase,  k##Postfix)             \
    DEFINE_LITERAL(QuantityType, Mega##UnitNameLowerCase,  M##Postfix)             \
    DEFINE_LITERAL(QuantityType, Giga##UnitNameLowerCase,  G##Postfix)

// SI base quantities
// https://en.wikipedia.org/wiki/International_System_of_Units#SI_base_units

#define DEFINE_QUANTITY_TRAIT(QuantityName) \
    namespace base {                        \
    struct QuantityName : BaseQuantity {};  \
    } // namespace base

namespace base {
struct Length : BaseQuantity {};
struct Duration : BaseQuantity {};
struct Timepoint : BaseQuantity {};
struct Mass : BaseQuantity {};
struct Current : BaseQuantity {};
struct TemperatureDelta : BaseQuantity {};
struct Temperature : BaseQuantity {};
struct Angle : BaseQuantity {};
struct SolidAngle : BaseQuantity {};
} // namespace base

using Length = Quantity<std::tuple<base::Length>, std::tuple<>>;

using Duration = Quantity<std::tuple<base::Duration>, std::tuple<>>;

// AddConstraint: Allow Timepoint + Duration, disallow Timepoint + Timepoint
using Timepoint = Quantity<std::tuple<base::Timepoint>, std::tuple<>, Duration>;

using Mass = Quantity<std::tuple<base::Mass>, std::tuple<>>;

using Current = Quantity<std::tuple<base::Current>, std::tuple<>>;

using TemperatureDelta = Quantity<std::tuple<base::TemperatureDelta>, std::tuple<>>;

using Temperature = Quantity<std::tuple<base::Temperature>, std::tuple<>, TemperatureDelta>;

// TODO: Add mol, candela

// 22 SI derived quantities
// https://en.wikipedia.org/wiki/International_System_of_Units#Derived_units

using Angle = Quantity<std::tuple<base::Angle>, std::tuple<>>;

using SolidAngle = Quantity<std::tuple<base::SolidAngle>, std::tuple<>>;

using Frequency = Quantity<std::tuple<>, std::tuple<base::Duration>>;

using Force = Quantity<std::tuple<base::Mass, base::Length>, std::tuple<base::Duration, base::Duration>>;

using Pressure = Quantity<std::tuple<base::Mass>, std::tuple<base::Duration, base::Duration, base::Length>>;

using Energy = Quantity<std::tuple<base::Mass, base::Length, base::Length>, std::tuple<base::Duration, base::Duration>>;

using Power = Quantity<
    std::tuple<base::Mass, base::Length, base::Length>,
    std::tuple<base::Duration, base::Duration, base::Duration>>;

using Charge = Quantity<std::tuple<base::Duration, base::Current>, std::tuple<>>;

using Voltage = Quantity<
    std::tuple<base::Mass, base::Length, base::Length>,
    std::tuple<base::Duration, base::Duration, base::Duration, base::Current>>;

using Capacitance = Quantity<
    std::tuple<base::Duration, base::Duration, base::Duration, base::Duration, base::Current, base::Current>,
    std::tuple<base::Mass, base::Length, base::Length>>;

using Resistance = Quantity<
    std::tuple<base::Mass, base::Length, base::Length>,
    std::tuple<base::Duration, base::Duration, base::Duration, base::Current, base::Current>>;

using Conductance = Quantity<
    std::tuple<base::Duration, base::Duration, base::Duration, base::Duration, base::Duration>,
    std::tuple<base::Duration, base::Duration, base::Duration>>;

// TODO: Finish

// More derived quantities
// https://en.wikipedia.org/wiki/International_System_of_Units#Coherent_and_non-coherent_SI_units

using Area = Quantity<std::tuple<base::Length, base::Length>, std::tuple<>>;

using Volume = Quantity<std::tuple<base::Length, base::Length, base::Length>, std::tuple<>>;

using Velocity = Quantity<std::tuple<base::Length>, std::tuple<base::Duration>>;

using Acceleration = Quantity<std::tuple<base::Length>, std::tuple<base::Duration, base::Duration>>;

// Impossible to create a Quantity out of non-base quantities
static_assert(TupleLike<std::tuple<>>);
static_assert(TupleLike<std::tuple<int>>);
static_assert(TupleLike<std::tuple<base::Duration>>);
static_assert(!TupleLike<int>);
static_assert(allElementsDerivedFromBaseQuantity(std::tuple<base::Duration>()));
static_assert(!allElementsDerivedFromBaseQuantity(std::tuple<Acceleration>()));
static_assert(BaseQuantityTuple<std::tuple<base::Duration>>);
static_assert(!BaseQuantityTuple<Frequency>);

// A valid quantity consisting of base quantities
static_assert(IsQuantity<Quantity<std::tuple<base::Duration>, std::tuple<>>>);

// Invalid quantities consisting of non-base quantities
// static_assert(IsQuantity<Quantity<Acceleration, std::tuple<>>>);
// static_assert(IsQuantity<Quantity<std::tuple<Acceleration>, std::tuple<>>>);

// Units

template<typename Quantity_, typename Ratio_>
struct Unit {
    using Quantity = Quantity_;
    using Ratio = Ratio_;
};

// SI units

DEFINE_UNITS(Length, Meter, meter, m)

struct Nanosecond : Unit<Duration, std::nano> {};
struct Microsecond : Unit<Duration, std::micro> {};
struct Millisecond : Unit<Duration, std::milli> {};
struct Second : Unit<Duration, std::ratio<1>> {};
struct Minute : Unit<Duration, std::ratio<60, 1>> {};
struct Hour : Unit<Duration, std::ratio<60 * 60, 1>> {};
struct Day : Unit<Duration, std::ratio<24 * 60 * 60, 1>> {};

DEFINE_LITERAL(Duration, Nanosecond, ns)
DEFINE_LITERAL(Duration, Microsecond, us)
DEFINE_LITERAL(Duration, Millisecond, ms)
DEFINE_LITERAL(Duration, Second, s)
DEFINE_LITERAL(Duration, Minute, min)
DEFINE_LITERAL(Duration, Hour, hr)
DEFINE_LITERAL(Duration, Day, day)

struct Microgram : Unit<Mass, std::nano> {};
struct Milligram : Unit<Mass, std::micro> {};
struct Gram : Unit<Mass, std::milli> {};
struct Kilogram : Unit<Mass, std::ratio<1>> {};
struct Ton : Unit<Mass, std::kilo> {};

DEFINE_LITERAL(Mass, Microgram, ug)
DEFINE_LITERAL(Mass, Milligram, mg)
DEFINE_LITERAL(Mass, Gram, g)
DEFINE_LITERAL(Mass, Kilogram, kg)
DEFINE_LITERAL(Mass, Ton, t)

DEFINE_UNITS(Current, Ampere, ampere, A)

struct Kelvin : Unit<Temperature, std::ratio<1>> {};

DEFINE_LITERAL(Temperature, Kelvin, K)

struct Celsius
    : Unit<
          Temperature,
          Conversion<static_cast<int64_t>(1e15L), static_cast<int64_t>(273.15 * 1e15L), static_cast<int64_t>(1e15L)>> {
};

// DEFINE_LITERAL(Temperature, Celsius, C)

struct dKelvin : Unit<TemperatureDelta, std::ratio<1>> {};

DEFINE_LITERAL(TemperatureDelta, dKelvin, dK)

struct dCelsius : Unit<TemperatureDelta, std::ratio<1>> {};

DEFINE_LITERAL(TemperatureDelta, dCelsius, dC)

// 22 SI derived units

struct Radian : Unit<Angle, std::ratio<1>> {};

DEFINE_LITERAL(Angle, Radian, rad)

struct Degree : Unit<Angle, ratiod<M_PI, 180.0>> {};

DEFINE_LITERAL(Angle, Degree, deg)

DEFINE_UNITS(Force, Newton, newton, N)

DEFINE_UNITS(Pressure, Pascal, pascal, Pa)

struct Bar : Unit<Pressure, std::ratio<100'000, 1>> {};
struct Millibar : Unit<Pressure, std::ratio<100, 1>> {};

DEFINE_LITERAL(Pressure, Bar, bar)
DEFINE_LITERAL(Pressure, Millibar, mbar)

DEFINE_UNITS(Power, Watt, watt, W)

DEFINE_UNITS(Energy, Joule, joule, J)

struct WattSecond : Unit<Energy, std::ratio<1>> {};
struct WattHour : Unit<Energy, std::ratio_multiply<Watt::Ratio, Hour::Ratio>> {};
struct KilowattHour : Unit<Energy, std::ratio_multiply<Kilowatt::Ratio, Hour::Ratio>> {};

DEFINE_LITERAL(Energy, WattSecond, Ws)
DEFINE_LITERAL(Energy, WattHour, Wh)
DEFINE_LITERAL(Energy, KilowattHour, kWh)

DEFINE_UNITS(Charge, Coulomb, coulomb, C)

DEFINE_UNITS(Voltage, Volt, volt, V)

DEFINE_UNITS(Capacitance, Farad, farad, F)

DEFINE_UNITS(Resistance, Ohm, ohm, Ohm)

// More derived SI units

struct SquareMillimeter : Unit<Area, std::ratio_multiply<Millimeter::Ratio, Millimeter::Ratio>> {};
struct SquareCentimeter : Unit<Area, std::ratio_multiply<Centimeter::Ratio, Centimeter::Ratio>> {};
struct SquareMeter : Unit<Area, std::ratio<1>> {};
struct SquareKilometer : Unit<Area, std::ratio_multiply<Kilometer::Ratio, Kilometer::Ratio>> {};

DEFINE_LITERAL(Area, SquareMillimeter, mm2)
DEFINE_LITERAL(Area, SquareCentimeter, cm2)
DEFINE_LITERAL(Area, SquareMeter, m2)
DEFINE_LITERAL(Area, SquareKilometer, km2)

struct CubicMeter : Unit<Volume, std::ratio<1>> {};

DEFINE_LITERAL(Volume, CubicMeter, m3)

struct MeterPerSecond : Unit<Velocity, std::ratio<1>> {};
struct KilometerPerHour : Unit<Velocity, std::ratio_divide<Kilometer::Ratio, Hour::Ratio>> {};

DEFINE_LITERAL(Velocity, MeterPerSecond, mps)
DEFINE_LITERAL(Velocity, KilometerPerHour, kmph)

struct MeterPerSquareSecond : Unit<Acceleration, std::ratio<1>> {};

struct NewtonMeter
    : Unit<
          Quantity<std::tuple<base::Length, base::Length, base::Mass>, std::tuple<base::Duration, base::Duration>>,
          std::ratio<1>> {};

DEFINE_LITERAL(NewtonMeter::Quantity, NewtonMeter, Nm)


// Imperial units

struct Inch : Unit<Length, std::ratio<10000, 25400>> {};
struct Feet : Unit<Length, std::ratio<10000, 3048>> {};
struct Yard : Unit<Length, std::ratio<10000, 9144>> {};
struct Mile : Unit<Length, std::ratio<1609344, 1000>> {};


// Operators

template <typename Q> requires IsQuantity<Q>
constexpr bool operator==(Q const lhs, Q const rhs) {
    return lhs._value == rhs._value;
}

template<typename Q> requires IsQuantity<Q>
constexpr auto operator<(Q const lhs, Q const rhs) {
    return lhs._value < rhs._value;
}

template<typename Q> requires IsQuantity<Q>
constexpr auto operator>(Q const lhs, Q const rhs) {
    return lhs._value > rhs._value;
}

template<typename Q> requires (IsQuantity<Q> && not HasAddConstraint<Q>)
constexpr Q operator+(Q const lhs, Q const rhs) {
    Q q;
    q._value = lhs._value + rhs._value;
    return q;
}

template<typename QLhs, typename QRhs> requires (
    IsQuantity<QLhs> && IsQuantity<QRhs> && std::is_same_v<typename QLhs::AddConstraint, QRhs>)
constexpr QLhs operator+(QLhs const lhs, QRhs const rhs) {
    QLhs q;
    q._value = lhs._value + rhs._value;
    return q;
}

template<typename QLhs, typename QRhs> requires (
    IsQuantity<QLhs> && IsQuantity<QRhs> && std::is_same_v<QLhs, typename QRhs::AddConstraint>)
constexpr QRhs operator+(QLhs const lhs, QRhs const rhs) {
    QRhs q;
    q._value = lhs._value + rhs._value;
    return q;
}

template <typename Q> requires IsQuantity<Q>
constexpr Q operator-(Q const q) {
    Q qNew;
    qNew._value = -q._value;
    return qNew;
}

template<typename QLhs, typename QRhs> requires(not (HasAddConstraint<QLhs> && HasAddConstraint<QRhs>))
constexpr QLhs operator-(QLhs const lhs, QRhs const rhs) {
    return lhs + (-rhs);
}

// E.g. TimePoint - TimePoint -> Duration
template<typename QLhs, typename QRhs> requires(
    std::is_same_v<typename QLhs::AddConstraint, typename QRhs::AddConstraint>
    && HasAddConstraint<QLhs> && HasAddConstraint<QRhs>)
constexpr typename QLhs::AddConstraint operator-(QLhs const lhs, QRhs const rhs) {
    typename QLhs::AddConstraint q;
    q._value = lhs._value - rhs._value;
    return q;
}

template <typename NumLhs, typename DenLhs, typename NumRhs, typename DenRhs> requires (
    IsQuantity<Quantity<NumLhs, DenLhs>> && IsQuantity<Quantity<NumRhs, DenRhs>>)
constexpr auto operator*(Quantity<NumLhs, DenLhs> const lhs, Quantity<NumRhs, DenRhs> const rhs) {
    constexpr auto num = std::tuple_cat(NumLhs(), NumRhs());
    constexpr auto den = std::tuple_cat(DenLhs(), DenRhs());
    constexpr auto tmp = removePairs(std::remove_cvref_t<decltype(num)>(num), std::remove_cvref_t<decltype(den)>(den));
    Quantity<
        std::remove_cvref_t<decltype(std::get<0>(tmp))>,
        std::remove_cvref_t<decltype(std::get<1>(tmp))>> q;
    q._value = lhs._value * rhs._value;
    return q;
}

template <typename Q> requires IsQuantity<Q>
constexpr auto operator*(Q const lhs, typename Q::Rep const scalar) {
    Q q = lhs;
    q._value *= scalar;
    return q;
}

template <typename Q> requires IsQuantity<Q>
constexpr auto operator*(typename Q::Rep const scalar, Q const lhs) {
    return lhs * scalar;
}

template <typename Q> requires IsQuantity<Q>
constexpr auto operator/(Q const lhs, typename Q::Rep const scalar) {
    Q q = lhs;
    q._value /= scalar;
    return q;
}

template <typename Num, typename Den, typename AddConstraint> requires IsQuantity<Quantity<Num, Den, AddConstraint>>
constexpr auto operator/(typename Quantity<Num, Den, AddConstraint>::Rep const scalar, Quantity<Num, Den, AddConstraint> const q) {
    Quantity<Den, Num, AddConstraint> qNew; // Invert Num and Den
    qNew._value = scalar / q._value;
    return qNew;
}

template<typename Q> requires IsQuantity<Q>
constexpr auto operator/(Q const lhs, Q const rhs) {
    return lhs._value / rhs._value;
}

template <typename NumLhs, typename DenLhs, typename NumRhs, typename DenRhs> requires (
    not SameQuantity<Quantity<NumLhs, DenLhs>, Quantity<NumRhs, DenRhs>>)
constexpr auto operator/(Quantity<NumLhs, DenLhs> const lhs, Quantity<NumRhs, DenRhs> const rhs) {
    constexpr auto num = std::tuple_cat(NumLhs(), DenRhs());
    constexpr auto den = std::tuple_cat(DenLhs(), NumRhs());
    constexpr auto tmp = removePairs(std::remove_cvref_t<decltype(num)>(num), std::remove_cvref_t<decltype(den)>(den));
    Quantity<
        std::remove_cvref_t<decltype(std::get<0>(tmp))>,
        std::remove_cvref_t<decltype(std::get<1>(tmp))>> q;
    q._value = lhs._value / rhs._value;
    return q;
}

namespace math {
template<typename Q>
    requires IsQuantity<Q>
constexpr auto abs(Q const q) {
    auto newQ = q;
    if (q._value < 0) {
        newQ._value = -newQ._value;
    }
    return newQ;
}

inline Angle::Rep sin(Angle const a) { return std::sin(a._value); }
inline Angle::Rep cos(Angle const a) { return std::cos(a._value); }
inline Angle::Rep tan(Angle const a) { return std::tan(a._value); }
inline Angle::Rep sinh(Angle const a) { return std::sinh(a._value); }
inline Angle::Rep cosh(Angle const a) { return std::cosh(a._value); }
inline Angle::Rep tanh(Angle const a) { return std::tanh(a._value); }

inline Angle asin(Angle::Rep const a) { return Angle::from<Radian>(std::asin(a)); }
inline Angle acos(Angle::Rep const a) { return Angle::from<Radian>(std::acos(a)); }
inline Angle atan(Angle::Rep const a) { return Angle::from<Radian>(std::atan(a)); }
inline Angle asinh(Angle::Rep const a) { return Angle::from<Radian>(std::asinh(a)); }
inline Angle acosh(Angle::Rep const a) { return Angle::from<Radian>(std::acosh(a)); }
inline Angle atanh(Angle::Rep const a) { return Angle::from<Radian>(std::atanh(a)); }

inline Angle atan2(Angle::Rep const y, Angle::Rep const x) {
    Angle a;
    a._value = std::atan2(y, x);
    return a;
}
} // namespace math

template <typename Q> requires IsQuantity<Q>
constexpr std::string_view q_name() {
    return "N/A";
}

template <>
constexpr std::string_view q_name<Length>() {
    return "Length";
}

template <>
constexpr std::string_view q_name<Duration>() {
    return "Duration";
}

std::ostream& operator<<(std::ostream& s, IsQuantity auto const q) {
    s.precision(10);
    s << q._value << " (" << q_name<std::remove_cvref_t<decltype(q)>>() << ")";
    return s;
}

} // namespace ccunits

#endif