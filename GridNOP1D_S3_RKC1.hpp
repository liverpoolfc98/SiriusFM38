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
    long a_Nints,
    int a_tauMins,
    double a_UBSDF // upper bound discount factor
) {
    assert(a_option && a_diff && a_s0 && a_tauMins && a_Nints);

    auto TTE = YearFrac(a_option->expirTime()) - YearFrac(a_t0);
    if (TTE <= 0) {
        throw std::runtime_error("option is already expired");
    }

    long Mints = (a_option->expirTime() - a_t0) / (a_tauMins * 60);
    auto M = Mints + 1;

    if (M > m_maxM) {
        throw std::runtime_error("too big M");
    }

    auto tau = TTE / Mints;
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

    auto StDS = std::sqrt(m_VS[M - 1]);

    auto B = m_ES[M - 1] + a_UBSDF * StDS;

    // printf("B = %f, Ty = %f\n", B, m_ts[M - 1]);
    // printf("Var = %f\n", StDS);

    auto h = B / a_Nints;

    h = a_s0 / round(a_s0 / h);
    if (!std::isfinite(h)) {
        throw std::invalid_argument("s0 is too close to zero");
    }

    B = h * a_Nints;

    auto N = a_Nints + 1; // number of points

    if (N > m_maxN) {
        throw std::invalid_argument("too big N");
    }

    auto last = m_grid + N * (M - 1);
    for (auto i = 0; i < N; ++i) {
        m_s[i] = i * h;
        last[i] = a_option->payoff(1, m_ts + M - 1, m_s + i);
    }

    // at low bound we always have constant boundary condition
    auto fa = last[0];

    // at the upper bound we use a const boundary condition if it's zero
    // otherwise we fix df / ds

    auto isNeumann = last[N - 1] != 0;
    auto UBC = isNeumann ? (last[N - 1] - last[N - 2]) : 0;

    for (auto j = 0; j < M - 1; ++j) {
        m_grid[j * N] = fa;
        m_grid[(j + 1) * N - 1] = 0;
    }

    auto D2 = 2 * h * h;
    
   //  #pragma acc kernels copyin(m_grid[0:N * M]) copyout(m_grid[0:N * M])
    for (auto j = M - 1; j >= 1; --j) {
        auto fj = m_grid + j * N;
        auto fjm1 = fj - N;
        auto tj = m_ts[j];
        auto rateAj = m_irpA.r(a_option->A(), tj);
        auto rateBj = m_irpB.r(a_option->B(), tj);
        auto C1 = (rateBj - rateAj) / 2 / h;

        fjm1[0] = fa;
	    // #pragma acc parallel loop copyin(fj[0:N]) copyout(fjm1[0:N])
	    #pragma omp parallel for 
        for (auto i = 1; i < N - 1; ++i) {
            auto si = m_s[i];
            auto fjim1 = fj[i - 1];
            auto fji = fj[i];
            auto fjip1 = fj[i + 1];
            auto vol = a_diff->sigma(si, tj);
            auto DfDt = rateBj * fji - C1 * si * (fjip1 - fjim1)
                - vol * vol / D2 * (fjip1 - 2 * fji + fjim1);
            fjm1[i] = fji - tau * DfDt;
        }
        // fjm1[N - 1] = isNeumann ? fjm1[N - 2] + UBC * h : UBC;
	
        if (a_option->IsAmerican()) {
            for (auto i = 0; i < N; ++i) {
                fjm1[i] = std::max<double>(fjm1[i], a_option->payoff(1, m_ts + j, m_s + i));
            }   
	    }
    }
    m_i0 = round(a_s0 / h);
    m_M = M, m_N = N;
}

template<class Diffusion1D,
         class AProvider,
         class BProvider,
         class AssetClassA,
         class AssetClassB>
std::tuple<double, double, double>
GridNOP1D<Diffusion1D, AProvider, BProvider, AssetClassA, AssetClassB>::getStats() const {
    if (m_M == 0 || m_N == 0) {
        throw std::runtime_error("run BI first");
    }

    auto h = m_s[1] - m_s[0];
    auto tau = m_ts[1] - m_ts[0];
    auto px = m_grid[m_i0];

    double delta = 0, gamma = 0;

    if (m_i0 > 0 && m_i0 < m_N - 1) {
	delta = (m_grid[m_i0 + 1] - m_grid[m_i0 - 1]) / 2 / h;
        gamma = (m_grid[m_i0 - 1] - 2 * m_grid[m_i0] + m_grid[m_i0 + 1]) / h / h;
    } else if (m_i0 == 0) {
	delta = (m_grid[m_i0 + 1] - m_grid[m_i0]) / h;
    } else {
	delta = (m_grid[m_i0] - m_grid[m_i0 - 1]) / h;
    }
    return std::make_tuple(px, delta, gamma);
}

}
