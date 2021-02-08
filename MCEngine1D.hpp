#pragma once

#include <cassert>
#include <stdexcept>
#include <random>
#include <cmath>

#include "MCEngine1D.h"
#include "Time.h"

namespace siriusFM {

template <class Diffusion1D,
          class AProvider,
          class BProvider,
          class AssetClassA,
          class AssetClassB>
inline void
MCEngine1D<Diffusion1D, AProvider, BProvider, AssetClassA, AssetClassB>::Simulate(
    time_t a_t0,
    time_t a_T,
    int a_tau_min,
    double a_s0,
    long a_P,
    Diffusion1D const* a_diff,
    AProvider const* a_rateA,
    BProvider const* a_rateB,
    AssetClassA a_A,
    AssetClassB a_B,
    bool a_isRN) {

    assert(a_diff && a_rateA && a_rateB && (a_t0 <= a_T) && a_P > 0
        && a_tau_min > 0 && a_A != AssetClassA::UNDEFINED && a_B != AssetClassB::UNDEFINED);
    
    double y0 = YearFrac(a_t0), yT = YearFrac(a_T), tau = a_tau_min / (365.25 * 1440);
    long L = ceil((yT - y0) / tau) + 1, P = a_P << 1;

    if (L * P <= m_MaxL * m_MaxP) {
        throw std::invalid_argument("too big L and P");
    }

    std::random_device rd{};
    std::mt19937_64 u{rd()};
    std::normal_distribution<> n01{0, 1};

    double stau = std::sqrt(tau);
    double tlast = yT - y0 - (L - 2) * tau, slast = std::sqrt(tlast);

    assert(tlast < tau && tlast > 0);

    for (long p = 0; p < a_P; ++p) {
        double* path0 = m_paths + (p << 1) * L;
        double* path1 = path0 + L;
        path1[0] = path0[0] = a_s0;

        double y = y0, sp0 = a_s0, sp1 = a_s0;
        for (long l = 1; l < L; ++l) {
            double delta = a_rateB->r(a_B, y) - a_rateA->r(a_A, y);
            double mu0 = a_isRN ? delta * sp0 : a_diff->mu(sp0, y);
            double mu1 = a_isRN ? delta * sp1 : a_diff->mu(sp1, y);
            double sigma0 = a_diff->sigma(sp0, y), sigma1 = a_diff->sigma(sp1, y);
            
            double inc = n01(u) * (l == L - 1 ? slast : stau);
            double incTime = l == L - 1 ? tlast : tau;

            sp0 = path0[l] = sp0 + mu0 * incTime + inc * sigma0;
            sp1 = path1[l] = sp1 + mu1 * incTime - inc * sigma1;
        }
    }

}

}