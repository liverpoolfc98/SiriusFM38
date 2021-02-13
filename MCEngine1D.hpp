#pragma once

#include <cassert>
#include <stdexcept>
#include <random>
#include <cmath>

#include "MCEngine1D.h"
#include "Time.h"

#define EPS 1e-10

namespace siriusFM {

template <class Diffusion1D,
          class AProvider,
          class BProvider,
          class AssetClassA,
          class AssetClassB,
          class PathEvaluator>
template <bool IsRN>
inline void
MCEngine1D<Diffusion1D, AProvider, BProvider, AssetClassA, AssetClassB, PathEvaluator>::Simulate(
    time_t a_t0,
    time_t a_T,
    int a_tau_min,
    long a_P,
    bool a_useTime,
    const Diffusion1D* a_diff,
    AProvider* a_rateA,
    BProvider* a_rateB,
    AssetClassA a_A,
    AssetClassB a_B,
    PathEvaluator* a_eval) {

    assert(a_diff && a_rateA && a_rateB && (a_t0 <= a_T) && a_P > 0
        && a_tau_min > 0 && a_A != AssetClassA::UNDEFINED
        && a_B != AssetClassB::UNDEFINED && a_eval);
    
    double tau = a_tau_min / (365.25 * 1440);

    long L = (a_T - a_t0) / (a_tau_min * 60)
        + (long)((a_T - a_t0) % (a_tau_min * 60) != 0) + 1;
    // L - number of points
    long P = a_P << 1;

    if (L > m_MaxL) {
        throw std::invalid_argument("too big L");
    }

    long PM = (m_MaxL * m_MaxP) / L;

    PM = PM & 1 ? PM ^ 1 : PM;

    assert(PM > 0);

    long PI = P / PM + static_cast<long>(P % PM != 0);

    P = PM * PI;

    long PMH = PM >> 1;

    std::random_device rd{};
    std::mt19937_64 u{a_useTime ? static_cast<long unsigned int>(time(nullptr)) : rd()};
    std::normal_distribution<> n01{0, 1};

    double stau = std::sqrt(tau);
    double tlast = YearFrac(a_T) - YearFrac(a_t0) - tau * (L - 2);
    double slast = std::sqrt(tlast);

    assert(tlast <= tau + EPS && tlast > 0);

    *m_ts = YearFrac(a_t0);

    for (long l = 1; l < L; ++l) {
        *(m_ts + l) = *(m_ts + l - 1) + (l == L - 1 ? tlast : tau);
    }

    for (long i = 0; i < PI; ++i) {
	#pragma omp parallel for
        for (long p = 0; p < PMH; ++p) {
            double* path0 = m_paths + (p << 1) * L;
            double* path1 = path0 + L;
            *path1 = *path0 = a_diff->s0();

            double y = YearFrac(a_t0), sp0 = a_diff->s0(), sp1 = a_diff->s0();
            for (long l = 1; l < L; ++l) {
                double delta = a_rateB->r(a_B, y) - a_rateA->r(a_A, y);
                double mu0 = IsRN ? delta * sp0 : a_diff->mu(sp0, y);
                double mu1 = IsRN ? delta * sp1 : a_diff->mu(sp1, y);      
                double inc = 0;
		#pragma omp critical
		{inc = n01(u);}
		inc *= l == L - 1 ? slast : stau;
                double incTime = l == L - 1 ? tlast : tau;

                sp0 = path0[l] = sp0 + mu0 * incTime + inc * a_diff->sigma(sp0, y);
                sp1 = path1[l] = sp1 + mu1 * incTime - inc * a_diff->sigma(sp1, y);
                y += tau;
            }
        }
        (*a_eval)(L, PM, m_paths, m_ts);
    }
}

}
