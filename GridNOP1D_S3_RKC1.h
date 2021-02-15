#pragma once

#include "VanillaOptions.h"
#include "MCOptionHedger1D.hpp"

namespace siriusFM {

template<class Diffusion1D,
         class AProvider,
         class BProvider,
         class AssetClassA,
         class AssetClassB>
class GridNOP1D {
public:
    GridNOP1D(const char* a_irpAFile, const char* a_irpBFile, long a_maxM, long a_maxN)
    :
    m_irpA(a_irpAFile),
    m_irpB(a_irpBFile),
    m_maxM(a_maxM),
    m_maxN(a_maxN),
    m_grid(new double[a_maxM * a_maxN]),
    m_ts(new double[a_maxM]),
    m_s(new double[a_maxN]),
    m_ES(new double[a_maxM]),
    m_VS(new double[a_maxM]),
    m_i0(0),
    m_M(0),
    m_N(0)
     {
        memset(m_grid, 0, sizeof(m_grid));
        memset(m_ts, 0, sizeof(m_ts));
        memset(m_s, 0, sizeof(m_s));
        memset(m_ES, 0, sizeof(m_ES));
        memset(m_VS, 0, sizeof(m_VS));
    }

    void RunBI(
        const Option<AssetClassA, AssetClassB>* a_option,
        time_t a_t0,
        const Diffusion1D* a_diff,
        double a_s0,
        long a_N = 500,
        int a_tauMins = 30,
        double a_UBSDF = 4.5
    );

    std::tuple<double, double, double> getStats() const;

    ~GridNOP1D() {
        delete []m_grid;
        delete []m_ts;
        delete []m_s;
        delete []m_ES;
        delete []m_VS;
    }
private:
    AProvider m_irpA;
    BProvider m_irpB;
    long m_maxM;
    long m_maxN;
    double* const m_grid;
    double* const m_ts;
    double* const m_s;
    double* const m_ES;
    double* const m_VS;
    int m_i0;
    long m_M;
    long m_N;
};


}