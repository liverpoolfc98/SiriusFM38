#pragma once

#include <cmath>
#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string.h>
#include <ctime>

#include "DiffusionGBM.h"
#include "IRProviderConst.h"
#include "MCEngine1D.hpp"
#include "VanillaOptions.h"
#include "MCOptionPricer1D.h"

namespace siriusFM {

template <class Diffusion1D,
          class AProvider,
          class BProvider,
          class AssetClassA,
          class AssetClassB>
std::pair<double, double> MCOptionPricer1D
<Diffusion1D, AProvider, BProvider, AssetClassA, AssetClassB>::price(
        Option<AssetClassA, AssetClassB>* a_option, time_t a_t0, int a_tauMins, long a_P) {

    assert(a_option && a_tauMins > 0 && a_P > 0);
    
    OPPathEval eval(a_option);

    m_mce.template Simulate<true>(a_t0, a_option->expirTime(), a_tauMins, a_P,
    m_useTime, m_diff, &m_irpA, &m_irpB, a_option->A(), a_option->B(), &eval);

    double price = eval.getPrice();

    price *= m_irpB.DT(a_option->B(), a_t0, a_option->expirTime());

    return {price, eval.getSTD() / price};
}

}
