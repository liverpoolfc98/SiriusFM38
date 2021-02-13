#pragma once

#include "MCOptionHedger1D.h"
#include "IRProviderConst.h"

namespace siriusFM {

template <class Diffusion1D,
          class AProvider,
          class BProvider,
          class AssetClassA,
          class AssetClassB>
std::tuple<double, double, double, double> MCOptionHedger1D<Diffusion1D, AProvider, BProvider, AssetClassA, AssetClassB>::SimulateHedging(
    Option<AssetClassA, AssetClassB>* a_option,
    time_t                            a_t0,
    int                               a_tauMins,
    long                              a_P,
    double                            a_C0,
    DeltaFunc const*                  a_DeltaFunc,
    double                            a_DeltaAcc
) {
    assert(a_option && a_tauMins > 0 && a_P > 0);
    
    OHPathEval eval(a_option, &m_irpA, &m_irpB, a_DeltaAcc, a_C0, a_DeltaFunc);

    m_mce.template Simulate<false>(a_t0, a_option->expirTime(), a_tauMins, a_P,
    m_useTime, m_diff, &m_irpA, &m_irpB, a_option->A(), a_option->B(), &eval);

    return eval.getStats();
}

}
