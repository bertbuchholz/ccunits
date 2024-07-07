# Introduction

ccunits is (yet another) C++ header-only, compile time units library. It's simple to use and equally simple to add new quantities and units. The quantity of a computation is automatically resolved. It requires C++20 for concepts and some other template applications.

# Basic usage

To use, just copy Units.h into your source directory.

## Example Coulomb's Law

Example using Coulomb's Law of the force between two charged particles:
```
// F_C = 1 / (4 pi epsilon_0) * (q_1 * q_2) / r^2
// epsilon_0: [Farad/m] = [C/(V*m)]
// q: Charge [Coulomb]
// r: Distance [m]

auto const epsilon_0 = 8.8541878176e-12_F / 1_m;
auto const r = 1_m;
auto const q_1 = 1_C;
auto const q_2 = 1_C;
Force const F_C = 1.0 / (4.0 * M_PI * epsilon_0) * q_1 * q_2 / (r * r);
std::cout << "Force F_C: << F_C.to<Newton>() << "\n";

// This works when the variables are declared constexpr
// static_assert(abs(F_C.to<Newton>() - 1.0f / (4.0f * M_PI * 8.8541878176e-12)) < 1e-15);
```

The quantities need to be known at compile time. The values can be compile time or runtime. The above example can be evaluated at compile time when the variables are declared constexpr. The order in which the calculation is written is not important, e.g., these are all equivalent:

```
Force const F_C1 = 1.0 / (4.0 * M_PI * epsilon_0) * q_1 * q_2 / (r * r);
Force const F_C2 = q_2 / (r * r) * q_1 / (4.0 * M_PI * epsilon_0);
Force const F_C3 = (1.0 / r) * q_1 / (4.0 * M_PI * epsilon_0) * q_2 / r;
```

Wrong units will result in a compilation error:
```
Force const F_C1 = 1.0 / (4.0 * M_PI * epsilon_0) * q_1 * q_2 / (r); // Note only 1 r
// Compiler:
// Cannot convert from 'ccunits::Quantity<std::tuple<ccunits::base::Mass,ccunits::base::Length,ccunits::base::Length>,
// std::tuple<ccunits::base::Duration,ccunits::base::Duration>,void>' to 'ccunits::Quantity<std::tuple<ccunits::base::Mass
// ccunits::base::Length>,std::tuple<ccunits::base::Duration,ccunits::base::Duration>,void>'
```

## Example workflow

Often, we have raw-value inputs from somewhere with known quantity and unit, e.g. config files or user input. Typically, these are first converted to ccunits quantities and then used safely. At the end, they can be converted back to raw values:
```
// Immediately convert inputs
auto userLength = Length::from<Meter>(getUserLengthInMeters());
auto userWeight = Mass::from<Kilogram>(getUserWeightInKilograms());

// Density does not need to be defined as a type (unless raw conversion is required)
auto humanDensity = 985_kg / 1_m3;
auto userVolume = userWeight / humanDensity;

// Yes, this doesn't make much sense
auto userArea = userWeight / userLength;

// Write as square meters
writeOutput(userArea.to<SquareMeter>());
```

## Values

```
// This is assumed for all examples.
using namespace ccunits;
using namespace ccunits::literals;

// Define variables either from the quantities ::from() function and the unit explicitely
auto length1 = Length::from<Kilometer>(1.0);

// ... or via the literal (which determines the quantity)
auto length2 = 2.0_km;

// Does not compile since the unit isn't known
// Length length3 = 5.0;

// Convert to other units of length
std::cout << Length::to<Millimeter>(length1) << "\n"; // prints 0.000'001

// Does not compile, quantity and units don't match
// auto length4 = Length::from<Hour>(1.0);

// Does not compile, can't convert a Length to a Duration
// std::cout << Duration::to<Hour>(length1) << "\n";
```

Note that it is not possible (or desired) to get the internal representation of the value. To convert a unit value to a raw number, the `Quantity<Unit>::to()` function needs to be used which always specifies explicitely which unit should be used for conversion. Unlike std::chrono, where amounts of units are passed around (`auto d = std::chrono::milliseconds(5);`), ccunits passes around quantities (duration, length etc.). `std::chrono` uses `d.count()` to get the number of milliseconds as an integer when this representation is needed, but it doesn't require to specify the unit at the time of conversion. So the user needs to remember what unit `d` is, instead of just needing to know that it's a duration (short of using a `duration_cast`).

I found it a lot easier to always think of the objects just as quantities and largely forget about their unit after initializing them. And when it's required to convert them back to a number (which makes the value lose its state as a quantity), to then make this conversion explicit by forcing the specification of the output unit (and avoid a possibly erroneous direct access to the stored value like `count()` for `std::chrono::duration`).

## Operations

Within same quantity:
```
auto length1 = 10_m;
auto length2 = 5.0_m;

auto length3 = length1 + length2; // 15 m

auto length4 = 10_m + 5_m;
```

Creating new quantities from operations:
```
auto area = 10_m * 5_m;             // 50 m^2

auto velocity = 10_m / 2_s;         // 5 m/s
auto acceleration = velocity / 5_s; // 1 m/s^2
```

## Quantities and units

The quantity is what is being measured (temperature, length, duration etc.) and the unit is how it's measured. Units can be converted from one representation to another with certain ratios (e.g., kilometer to millimeter has a ratio of 1 million to 1).

### Defining quantities

Defining a new quantity, for example Velocity (which already exists):
```
using Velocity = Quantity<std::tuple<base::Length>, std::tuple<base::Duration>>;
```
This creates a new quantity called `Velocity`. The template parameters are the numerator (Length) and denominator (Duration) as a velocity is defined as a length (distance) over time. The tuple parameters must be defined in base quantities (from the `ccunits::base` namespace). This is enforced when defining a new quantity.

### Defining units

Defining a new unit (for the quantity we just defined):
```
struct MeterPerSecond : Unit<Velocity, std::ratio<1>> {};
```
This defines the unit `MeterPerSecond` which is a unit to measure the quantity `Velocity`. The ratio to relate it to other units of `Velocity` is `1`, which means it's essentially the base unit.

You can define a literal using the `DEFINE_LITERAL` macro:
```
DEFINE_LITERAL(Velocity, MeterPerSecond, mps)
```

This define the literal as `mps`.

This unit can now be used:
```
auto velocity = 4.2_mps; // 4.2 m/s
```

Further units can then be defined as related to the base:
```
struct KilometerPerHour : Unit<Velocity, std::ratio_divide<Kilometer::Ratio, Hour::Ratio>> {};
```

This creates a new unit for velocity, `KilometerPerHour`, which relates to the base as "kilometer / hour", which is expressed in the ratio: `std::ratio_divide<Kilometer::Ratio, Hour::Ratio>`. This could also have simply been written as `1000 / 3600`, that is, "kilometer expressed in the base meter divided by an hour expressed in the base second, i.e., 3600", but showing which units relate makes it more expressive and easier to follow.

### Relations

Quantities are related via mathematical operations, for example, length is related to velocity and duration as `length = velocity * duration` and `velocity = length / duration`. These relations between quantities don't need to be explicitely defined. This makes it possible to write:
```
auto velocity = 5_m / 2_s;        // 2.5 m/s
auto length = velocity * 1_min;   // 2.5 m/s * 60 s = 150 m
auto duration = 120_km / 60_kmph; // 120 km / 60 km/h = 2 h
```

To verify the quantity, auto can be replaced by the expected quantity:
```
Velocity velocity = 5_m / 2_s;
```

This would not compile:
```
Acceleration acceleration = 5_m / 2_s;
```

The unit of a quantity does not matter in these operations. When needed, the value can be extracted into a raw value again using the `.to<Quantity>()` function:
```
Velocity velocity = 5_m / 2_s;
velocity.to<MeterPerSecond>();     // 2.5
velocity.to<KilometerPerSecond>(); // 0.0025
```

### Math/Trigonometry functions

For some quantities, related math functions are defined. For example for `Angle`, the trigometric functions are defined:
```
float cosOfPiHalf = std::cos(M_PI * 0.5);
float cosOf90Degrees = ccunits::math::cos(90.0_deg);

// The inverse functions evaluate to an angle
auto angleFromAtan = ccunits::math::atan2(1, 1);
```

### Special quantities

#### Duration

For `Duration`, an implicit conversion from `std::chrono::duration` exists:
```
Duration durationFromChrono = 1s;
```

When used in an expression, `std::chrono::duration` needs to be constructed explicitly:
```
auto velocityWithChrono = 60_km / Duration(1h);
```

ccunits `Duration`'s literals have the added underscore:
```
auto ccunitsDuration = 1_h;
// vs.
Duration durationFromChrono = 1h;
```

#### Temperature vs TemperatureDelta and Timepoint vs Duration

When people say temperature, they usually mean a temperature point ("It's 10 degrees Celsius.") and a conversion of that temperature point to, for example, Kelvin is straight forward. However, the difference between two temperatures is not a temperature (point) but a temperature difference due to the zero point in the scale. This is the same difference between a duration and a time point. This means, the following operations are allowed:
```
TemperatureDelta +/- TemperatureDelta -> TemperatureDelta
Temperature +/- TemperatureDelta -> Temperature
Temperature - Temperature = TemperatureDelta
```
It's not possible to add two Temperatures, in the same way it doesn't make sense to add two time points (see also point vs. vector in geometry).
