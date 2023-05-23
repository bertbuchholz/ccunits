#include <iostream>
#include <map>
#include <string>
#include <chrono>
#include <ratio>
#include <cmath>

using namespace std::chrono_literals;

template <long int factor_, long int offset_, long int scale_>
struct Conversion {
    constexpr static long int factor = factor_;
    constexpr static long int offset = offset_;
    constexpr static long int scale = scale_;
};

// Duration
using Nanosecond = std::nano;
using Microsecond = std::micro;
using Millisecond = std::milli;
using Second = std::ratio<1>;
using Minute = std::ratio<60>;
using Hour = std::ratio<3600>;
using Day = std::ratio<3600 * 24>;

// Distance
using Nanometer = std::nano;
using Micrometer = std::micro;
using Millimeter = std::milli;
using Centimeter = std::centi;
using Meter = std::ratio<1>;
using Kilometer = std::kilo;

// Mass
using Gram = std::milli;
using Kilogram = std::ratio<1>;

// Area
using SquareMeter = std::ratio<1>;
using SquareMillimeter = std::ratio_multiply<Millimeter, Millimeter>;

// Speed
using MetersPerSecond = std::ratio<1>;
using KilometersPerHour = std::ratio_divide<Kilometer, std::chrono::hours::period>;

// Angles
using Radian = std::ratio<1>;
using Degree = std::ratio<static_cast<uint64_t>(M_PI * 1e10), static_cast<uint64_t>(180 * 1e10)>;

// Temperature
using Kelvin = std::ratio<1>;
using Celsius = Conversion<static_cast<uint64_t>(1 * 1e10), static_cast<uint64_t>(273.15 * 1e10), static_cast<uint64_t>(1e10)>;

// Energy
using Joule = std::ratio<1>;

// Power
using Watt = std::ratio<1>;
using Kilowatt = std::kilo;
using Megawatt = std::mega;

// Force
using Newton = std::ratio<1>;

// Acceleration
using MetersPerSecondSquared = std::ratio<1>;

// Momentum
using NewtonSecond = std::ratio<1>;
using KilogramMeterPerSecond = std::ratio<1>;


template <typename UnitType>
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
    constexpr static Rep to(const UnitType& unit) {
        return unit._value * static_cast<Rep>(T::den) / static_cast<Rep>(T::num);
    }
    
    template<typename T, std::enable_if_t<std::is_integral<decltype(T::factor)>::value, bool> = true>
    constexpr static Rep to(const UnitType& unit) {
        return unit._value / (static_cast<Rep>(T::factor) / static_cast<Rep>(T::scale))
            - (static_cast<Rep>(T::offset) / static_cast<Rep>(T::scale));
    }
};


// using Duration = std::chrono::duration<double>;
class Duration : public Unit<Duration> {};

class Distance : public Unit<Distance> {};
class Area : public Unit<Area>{};
class Speed : public Unit<Speed>{};
class Acceleration : public Unit<Acceleration> {};
class Mass : public Unit<Mass> {};
class Angle : public Unit<Angle>{};
class Temperature : public Unit<Temperature>{};
class Momentum : public Unit<Momentum>{};
class Force : public Unit<Force>{};
class Power : public Unit<Power>{};
class Energy : public Unit<Energy>{};

#define DEFINE_SCALAR_MULTIPLICATION(UnitType) UnitType operator*(const UnitType& lhs, const float rhs) { \
    UnitType u; \
    u._value = lhs._value * rhs; \
    return u; \
} \
\
UnitType operator*(const float lhs, const UnitType& rhs) { \
    UnitType u; \
    u._value = rhs._value * lhs; \
    return u; \
}

#define DEFINE_ADDITION_OPERATOR(UnitType) UnitType operator+(const UnitType& lhs, const UnitType& rhs) { \
    UnitType r; \
    r._value = lhs._value + rhs._value; \
    return r; \
}

#define DEFINE_SUBTRACTION_OPERATOR(UnitType) UnitType operator-(const UnitType& lhs, const UnitType& rhs) { \
    UnitType r; \
    r._value = lhs._value - rhs._value; \
    return r; \
}

#define DEFINE_MULTIPLICATION_OPERATOR(UnitTypeResult, UnitType1, UnitType2) \
 UnitTypeResult operator*(const UnitType1& lhs, const UnitType2& rhs) { \
    UnitTypeResult r; \
    r._value = lhs._value * rhs._value; \
    return r; \
} \
\
UnitTypeResult operator*(const UnitType2& lhs, const UnitType1& rhs) { \
    UnitTypeResult r; \
    r._value = lhs._value * rhs._value; \
    return r; \
}

#define DEFINE_MULTIPLICATION_SQUARE_OPERATOR(UnitTypeResult, UnitType) UnitTypeResult operator*(const UnitType& lhs, const UnitType& rhs) { \
    UnitTypeResult r; \
    r._value = lhs._value * rhs._value; \
    return r; \
}

#define DEFINE_DIVISION_OPERATOR(UnitTypeResult, UnitType1, UnitType2) UnitTypeResult operator/(const UnitType1& lhs, const UnitType2& rhs) { \
    UnitTypeResult r; \
    r._value = lhs._value / rhs._value; \
    return r; \
}

DEFINE_SCALAR_MULTIPLICATION(Duration)
DEFINE_SCALAR_MULTIPLICATION(Distance)
DEFINE_SCALAR_MULTIPLICATION(Area)
DEFINE_SCALAR_MULTIPLICATION(Speed)
DEFINE_SCALAR_MULTIPLICATION(Acceleration)
DEFINE_SCALAR_MULTIPLICATION(Mass)
DEFINE_SCALAR_MULTIPLICATION(Angle)
DEFINE_SCALAR_MULTIPLICATION(Temperature)
DEFINE_SCALAR_MULTIPLICATION(Momentum)
DEFINE_SCALAR_MULTIPLICATION(Force)
DEFINE_SCALAR_MULTIPLICATION(Power)
DEFINE_SCALAR_MULTIPLICATION(Energy)

DEFINE_ADDITION_OPERATOR(Distance)
DEFINE_ADDITION_OPERATOR(Angle)

DEFINE_MULTIPLICATION_SQUARE_OPERATOR(Area, Distance)
DEFINE_DIVISION_OPERATOR(Speed, Distance, Duration)
DEFINE_MULTIPLICATION_OPERATOR(Distance, Duration, Speed)
DEFINE_MULTIPLICATION_OPERATOR(Energy, Power, Duration)
DEFINE_MULTIPLICATION_OPERATOR(Energy, Force, Distance)
DEFINE_DIVISION_OPERATOR(Power, Energy, Duration)
DEFINE_MULTIPLICATION_OPERATOR(Force, Mass, Acceleration)
DEFINE_DIVISION_OPERATOR(Acceleration, Speed, Duration)
DEFINE_MULTIPLICATION_OPERATOR(Momentum, Mass, Speed)
DEFINE_DIVISION_OPERATOR(Force, Momentum, Duration)


namespace units {

Area::Rep sin(const Angle& a) {
    return std::sin(a._value);
}

Area::Rep cos(const Angle& a) {
    return std::cos(a._value);
}

Angle asin(const Area::Rep value) {
    Angle a;
    a._value = std::asin(value);
    return a;
}

Angle acos(const Area::Rep value) {
    Angle a;
    a._value = std::acos(value);
    return a;
}

}

#define DEFINE_LITERAL(UnitType, Unit, Postfix) \
constexpr UnitType operator""_##Postfix(const long double value) noexcept { \
    return UnitType::from<Unit>(value); \
} \
\
constexpr UnitType operator""_##Postfix(const unsigned long long int value) noexcept { \
    return UnitType::from<Unit>(value); \
}
DEFINE_LITERAL(Duration, Nanosecond, ns)
DEFINE_LITERAL(Duration, Microsecond, us)
DEFINE_LITERAL(Duration, Millisecond, ms)
DEFINE_LITERAL(Duration, Second, s)
DEFINE_LITERAL(Duration, Minute, min)
DEFINE_LITERAL(Duration, Hour, h)

DEFINE_LITERAL(Distance, Millimeter, mm)
DEFINE_LITERAL(Distance, Meter, m)
DEFINE_LITERAL(Distance, Kilometer, km)

DEFINE_LITERAL(Angle, Radian, rad)
DEFINE_LITERAL(Angle, Degree, deg)

DEFINE_LITERAL(Speed, MetersPerSecond, mps)

DEFINE_LITERAL(Temperature, Kelvin, K)
DEFINE_LITERAL(Temperature, Celsius, C)

DEFINE_LITERAL(Energy, Joule, J)

DEFINE_LITERAL(Power, Watt, W)

DEFINE_LITERAL(Force, Newton, N)

DEFINE_LITERAL(Mass, Gram, gr)
DEFINE_LITERAL(Mass, Kilogram, kg)

int main() {
    //const auto someDistance = 1_km;

    constexpr float someDistanceInKilometers = 0.001f;

    constexpr auto anotherDistance = Distance::from<Kilometer>(someDistanceInKilometers);

    std::cout << anotherDistance._value << "\n";

    const float anotherDistanceInMeters = Distance::to<Millimeter>(anotherDistance);

    std::cout << anotherDistanceInMeters << "\n";

    const auto area = Distance::from<Meter>(1) * Distance::from<Millimeter>(1);
    std::cout << "Area explicit: " << Area::to<SquareMeter>(area) << "\n";
    
    const auto area2 = 1_m * 1_mm;
    std::cout << "Area with literals: " << Area::to<SquareMeter>(area2) << "\n";

    std::cout << Area::to<SquareMillimeter>(area) << "\n";

    // TODO: Add automatic conversion from std::chrono::duration to our duration

    const auto speed = Distance::from<Kilometer>(1) / 1.0_s;
    std::cout << "1 kmps to mps: " << Speed::to<MetersPerSecond>(speed) << "\n";
    std::cout << "1 kmps to kmph: " << Speed::to<KilometersPerHour>(speed) << "\n";
    
    std::cout << "10mps * 5s: " << Distance::to<Meter>(10_mps * 5.0_s) << "\n";
    
    const auto speedOfLight = Speed::from<MetersPerSecond>(299'792'458);
    std::cout << "Speed of light * 1ns in cm: " << Distance::to<Centimeter>(speedOfLight * 1.0_ns) << " cm\n";

    std::cout << Distance::to<Kilometer>(30.4_m) << "\n";
    std::cout << Distance::to<Kilometer>(30_km) << "\n";
    
    constexpr Angle pi = Angle::from<Radian>(M_PI);
    
    std::cout << "45 deg in radians: " << Angle::to<Radian>(45.0_deg) << "\n";
    std::cout << "90 deg in radians: " << Angle::to<Radian>(90.0_deg) << "\n";
    std::cout << "180 deg in radians: " << Angle::to<Radian>(180.0_deg) << "\n";
    std::cout << "pi rad in degrees: " << Angle::to<Degree>(3.14159_rad) << "\n";
    std::cout << "pi in rad: " << Angle::to<Radian>(pi) << "\n";
    std::cout << "pi + 90 deg in degrees: " << Angle::to<Degree>(pi + 90.0_deg) << "\n";
    
    std::cout << "cos(M_PI * 0.5): " << std::cos(M_PI * 0.5) << "\n";
    std::cout << "cos(90 deg): " << units::cos(90.0_deg) << "\n";
    std::cout << "acos(0) in degrees: " << Angle::to<Degree>(units::acos(0)) << "\n";
    
    Temperature temp = Temperature::from<Kelvin>(5.0);
    std::cout << "Temp: " << Temperature::to<Celsius>(temp) << "\n";
    std::cout << "Temp 5K in Celsius: " << Temperature::to<Celsius>(5.0_K) << "\n";
    std::cout << "Temp 5K in Kelvin: " << Temperature::to<Kelvin>(5.0_K) << "\n";
    std::cout << "Temp 20C in Celsius: " << Temperature::to<Celsius>(20.0_C) << "\n";
    std::cout << "Temp 20C in Kelvin: " << Temperature::to<Kelvin>(20.0_C) << "\n";
    
    // Doesn't compile because 20_kg * 5_mps is not a known size, needs parentheses
    // Acceleration::to<MetersPerSecondSquared>(20.0_kg * 5.0_mps / 1.0_s)
    std::cout << "Acceleration 5mps/1s = 5m/s^2: " << Acceleration::to<MetersPerSecondSquared>(5.0_mps / 1.0_s) << "\n";
    std::cout << "Momentum 10kg * 5mps = 50Ns: " << Momentum::to<NewtonSecond>(10.0_kg * 5_mps) << "\n";
    
    std::cout << "Force 20kg * 5mps/s = 100N: " << Force::to<Newton>(20.0_kg * 5.0_mps / 1.0_s) << "\n";
    
    std::cout << "Energy 10N * 5m = 50J: " << Energy::to<Joule>(10.0_N * 5.0_m) << "\n";
    std::cout << "Energy 10W * 1s = 10Ws: " << Energy::to<Watt>(10.0_W * 1.0_s) << "\n";
    
    std::cout << "Power 10Ws / 5s = 2W: " << Power::to<Watt>(10.0_W * 1.0_s / 5.0_s) << "\n";
    std::cout << "Power 10kWh / 5h = 2kW: " << Power::to<Kilowatt>(10.0_W * 1000.0f * 1.0_h / 5.0_h) << "\n";
}