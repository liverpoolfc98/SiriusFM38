#pragma once

#include "GridNOP1D_S3_RKC1.h"
#include "Time.h"

namespace siriusFM {

template<class Diffusion1D,
         class AProvider,
         class BProvider,
         class AssetClassA,
         class AssetClassB>
void GridNOP1D<Diffusion1D, AProvider, BProvider, AssetClassA, AssetClassB>::RunBI(
    const Option<AssetClassA, AssetClassB>* a_option,
    time_t a_t0,
    const Diffusion1D* a_diff,
    // Grid params
    double a_s0,
    long a_N,
    int a_tauMins,
    double a_UBSDF // upper bound discount factor
) {
    assert(a_option && a_diff && a_s0 && a_tauMins);

    double TTE = YearFrac(a_option->expirTime()) - YearFrac(a_t0);
    if (TTE <= 0) {
        throw std::runtime_error("option is already expired");
    }

    long Mints = (a_option->expirTime() - a_t0) / (a_tauMins * 60);
    long M = Mints + 1;

    if (M > m_maxM) {
        throw std::runtime_error("too big M");
    }

    double tau = TTE / Mints;
    double integrAB = 0;

    *m_ts = YearFrac(a_t0);
    *m_ES = a_s0;
    *m_VS = 0;

    for (auto j = 1; j < M; ++j) {
        m_ts[j] = m_ts[j - 1] + tau;
        auto rA = m_irpA.r(a_option->A(), m_ts[j]);
        auto rB = m_irpB.r(a_option->B(), m_ts[j]);
        auto rateDiff = std::max<double>(rB - rA, 0);

        integrAB += rateDiff * tau;

        if (j < M - 1) {
            m_ES[j + 1] = a_s0 * std::exp(integrAB);
            auto sigma = a_diff->sigma(m_ES[j], m_ts[j]);
            m_VS[j + 1] = m_VS[j] + sigma * sigma * tau;
        }
    }

    double StDS = std::sqrt(m_VS[M - 1]);

    double B = m_ES[M - 1] + a_UBSDF * StDS;

    printf("B = %f, Ty = %f\n", B, m_ts[M - 1]);
    printf("Var = %f\n", StDS);

    double EST = 0;
}

}
