# Introduction

ccunits is (yet another) C++ header-only units library. It's simple to use and equally simple to add new quantities, units and relations between the quantities.

# Basic usage

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

Note that it is not possible (or desired) to get the internal representation of the value. To convert a unit value to a number, the `Quantity<Unit>::to()` function needs to be used which always specifies explicitely which unit should be used for conversion. Unlike std::chrono, where amounts of units are passed around (`auto d = std::chrono::milliseconds(5);`), ccunits passes around quantities (duration). `std::chrono` uses `d.count()` to get the number of milliseconds as an integer when this representation is needed, but it doesn't require to specify the unit at the time of conversion. So the user needs to remember what unit `d` is, instead of just needing to know that it's a duration (short of using a `duration_cast`).

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
auto area = 10_m * 5_m;          // 50 m^2

auto speed = 10_m / 2_s;         // 5 m/s
auto acceleration = speed / 5_s; // 1 m/s^2
```
How these quantities and units are defined and relate to each other needs to be specified and is explained below.

## Quantities and units

The quantity is what is being measured (temperature, length, duration etc.) and the unit is how it's measured. Units can be converted from one representation to another with certain ratios (e.g., kilometer to millimeter has a ratio of 1 million to 1).

### Defining quantities

Defining a new quantity:
```
REGISTER_QUANTITY(Speed)
```
This creates a new quantity called `Speed` that can then be linked to units and related to other quantities.

### Defining units

Defining a new unit:
```
DEFINE_UNIT_WITH_RATIO(Speed, MetersPerSecond, mps, std::ratio<1>)
```
This defines the unit `MetersPerSecond` which is a unit to measure the quantity `Speed`. Its literal is `mps` and the ratio to relate it to other units of `Speed` is `1`, which means it's essentially the base unit.

This unit can now be used:
```
auto speed = 4.2_mps; // 4.2 m/s
```

Further units should then be related to the base:
```
DEFINE_UNIT_WITH_RATIO(Speed, KilometersPerHour, kmph, std::ratio_divide<Kilometer::Ratio COMMA std::chrono::hours::period>)
```

This creates the unit of speed `KilometersPerHour`, which relates to the base as "kilometer / hour", which is expressed in the ratio: `std::ratio_divide<Kilometer::Ratio COMMA std::chrono::hours::period>`. This could also have simply been written as `1000 / 3600`, that is, "kilometer expressed in the base meter divided by an hour expressed in the base second, i.e., 3600", but showing which units relate makes it more expressive and easier to follow.

The literal is `kmph`. The COMMA here is just a detail to deal with commas in macros.

### Relations

// TODO: Show the creation of the relation between length, time and speed

Quantities are related via mathematical operations, for example, length is related to speed and duration as `length = speed * duration` and `speed = length / duration`. These relations between quantities must be explicitely defined.

Defining the relation of  length, speed and duration:
```
DEFINE_RELATION(Length, Speed, Duration)
```
This creates the relations:
- `length = speed * duration`
- `length = duration * speed`
- `duration = length / speed`
- `speed = length / duration`

Assuming that some units were set up for the used quantities, it's possible to then write:
```
auto speed = 5_m / 2_s;           // 2.5 m/s
auto length = speed * 1_m;        // 2.5 m/s * 60 s = 150 m
auto duration = 120_km / 60_kmph; // 120 km / 60 km/h = 2 h
```

The unit of a quantity does not matter when relating quantities.

### Math functions

For some quantities, related math functions are defined. For example for `Angle`, the trigometric functions are defined:
```
float cosOfPiHalf = std::cos(ccunits::constants::pi * 0.5);
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

`std::chrono::duration` can also be used directly in an expression:
```
auto speedWithChrono = 60_km / 1h;
```

ccunits `Duration`'s literals have the added underscore:
```
auto ccunitsDuration = 1_h;
// vs.
Duration durationFromChrono = 1h;
```

#### Temperature

When people say temperature, they usually mean a temperature point ("It's 10 degrees Celsius.") and a conversion of that temperature point to, for example, Kelvin is straight forward. However, the difference between two temperatures is not a temperature (point) but a temperature difference due to the zero point in the scale. This is the same different between a duration and a time point. This means, the following operations are allowed:
```
TemperatureDifference +/- TemperatureDifference -> TemperatureDifference
Temperature +/- TemperatureDifference -> Temperature
```
It's not possible to add two Temperatures, in the same way it doesn't make sense to add two time points (see also point vs. vector in geometry).

# Reference

// TODO: Show the different macros for unit creation and what they do (what they set up for each unit) and
//       what registering a quantity does

## Quantities

`REGISTER_QUANTITY(QuantityName)` registers a new quantity. It defines the following things for this quantity:
- Scalar multiplication
- Scalar division
- Addition
- Subtraction
- Negative operator
- Comparisons
- Self division (leads to unit-less number)

For quantities that should not have all of these properties, the quantity can be defined like so:
```
class Name : public Quantity<QuantityName> {};
```
and desired properties can be added as needed, e.g.:
```
DEFINE_SCALAR_DIVISION(QuantityName)
DEFINE_COMPARISONS(QuantityName)
```

## Units

There are multiple macros to define a unit, they differ in the way they set up the ratio.

- `DEFINE_UNIT(QuantityType_, UnitName, Literal, Numerator, Denominator)`
  Nominator and denominator need to be integers
- `DEFINE_UNIT_WITH_RATIO(QuantityType_, UnitName, Literal, Ratio)`
  Ratio is a std::ratio
- `DEFINE_UNIT_REAL_RATIO(QuantityType_, UnitName, Literal, Numerator, Denominator)`
  Nominator and denominator can be real numbers
- `DEFINE_UNIT_OFFSET(QuantityType_, UnitName, Literal, Factor, Offset)`
  Factor is the ratio in a single number, Offset is a difference between this unit and the base

## Relations

`DEFINE_RELATION(QuantityTypeResult, QuantityType1, QuantityType2)` defines relations between these types in the following way:
- QuantityType1 * QuantityType2 -> QuantityTypeResult
- QuantityType2 * QuantityType1 -> QuantityTypeResult
- QuantityTypeResult / QuantityType1 -> QuantityType2
- QuantityTypeResult / QuantityType2 -> QuantityType1

## Math functions

TODO