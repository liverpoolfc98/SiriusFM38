#pragma once

#include <cmath>

namespace siriusFM {

inline double YearFrac(time_t time) {
    constexpr double secYear = 365.25 * 86400;
    return 1970 + time / secYear;
}

}