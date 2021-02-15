#pragma once

#include <cmath>
#include <cassert>

namespace siriusFM {
    double Phi(double x) {
        return 0.5 * (1 + erf(x / M_SQRT2));
    }
    double BSMPriceCall(double a_s0, double a_K, double a_Ty,
        double a_sigma, double a_rateA, double a_rateB) {
        assert(a_s0 > 0 && a_K > 0 && a_sigma > 0);
        double x1 = (std::log(a_s0 / a_K) + a_Ty * (a_rateB - a_rateA
            + a_sigma * a_sigma / 2)) / a_sigma / sqrt(a_Ty);
        double x2 = (std::log(a_s0 / a_K) + a_Ty * (a_rateB - a_rateA
            - a_sigma * a_sigma / 2)) / a_sigma / sqrt(a_Ty);
        return a_s0 * std::exp(-a_rateA * a_Ty) * Phi(x1)
            - a_K * std::exp(-a_rateB * a_Ty) * Phi(x2);
    }
    double BSMPricePut(double a_s0, double a_K, double a_Ty,
        double a_sigma, double a_rateA, double a_rateB) {
        assert(a_s0 > 0 && a_K > 0 && a_sigma > 0);
        return -a_s0 + a_K * std::exp(-a_rateB * a_Ty)
            + BSMPriceCall(a_s0, a_K, a_Ty, a_sigma, a_rateA, a_rateB);
    }
    double DeltaCall(double a_s0, double a_K, double a_Ty,
        double a_sigma, double a_rateA, double a_rateB) {
        assert(a_s0 > 0 && a_K > 0 && a_sigma > 0);
        double x1 = (std::log(a_s0 / a_K) + a_Ty * (a_rateB - a_rateA
            + a_sigma * a_sigma / 2)) / a_sigma / sqrt(a_Ty);
        return Phi(x1);
    }
    double DeltaPut(double a_s0, double a_K, double a_Ty,
        double a_sigma, double a_rateA, double a_rateB) {
        return DeltaCall(a_s0, a_K, a_Ty, a_sigma, a_rateA, a_rateB) - 1;
    }
}

