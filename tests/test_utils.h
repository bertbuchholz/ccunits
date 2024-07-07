#include "Units.h"

template <typename Q>
bool isNear(Q const q1, Q const& q2, Q const epsilon) {
    return ccunits::math::abs(q1 - q2) < epsilon;
}

